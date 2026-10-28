#!/usr/bin/env python3
"""CH4-only focused capture measurements; no casewise tested-edge alignment.

Run with Python, NumPy, and SciPy; --out selects a new results directory.
The midpoint and affine estimators adapt the old probe's pure functions, not
its globals, matrix, expected counts, or tested-edge calibration.
"""
import argparse
import csv
import hashlib
import json
from collections import Counter
from pathlib import Path
import warnings

import numpy as np
from scipy import optimize, signal
from scipy.io import wavfile

OUT = Path(__file__).resolve().parent
KIT = OUT.parent.parent / "ch34-focused-probe"
HARDWARE = KIT.parent / "hardware/ew_png0d.wav"
CPU = 4194304
PERIOD = 1536
MODELS = {"P+D+R": (1, 1, 1), "P+R": (1, 0, 1), "P": (1, 0, 0),
          "D+R": (0, 1, 1), "R": (0, 0, 1), "0": (0, 0, 0)}


def stats(x):
    x = np.asarray(x, dtype=float)
    return {"n": len(x), "mean": float(x.mean()), "std": float(x.std()),
            "min": float(x.min()), "max": float(x.max()),
            "rms": float(np.sqrt(np.mean(x*x)))} if len(x) else {"n": 0}


def wrap(x):
    return (np.asarray(x) + PERIOD/2) % PERIOD - PERIOD/2


def read(path):
    with warnings.catch_warnings():
        warnings.simplefilter("ignore", wavfile.WavFileWarning)
        rate, x = wavfile.read(path)
    return rate, x.astype(float) / 2**(np.iinfo(x.dtype).bits-1)


def halfedge(x, rate, k, flank=3, fraction=.5, baseline="linear"):
    """Interpolated step crossing between extrapolated local baselines."""
    pre = np.arange(k-flank-1, k-1)
    post = np.arange(k+3, k+flank+3)
    degree = 1 if baseline == "linear" else 0
    b = np.polyfit(pre-k, x[pre], degree)
    a = np.polyfit(post-k, x[post], degree)
    ii = np.arange(k-2, k+4)
    level = (1-fraction)*np.polyval(b, ii-k)+fraction*np.polyval(a, ii-k)
    y = x[ii]-level
    step = np.polyval(a, .5)-np.polyval(b, .5)
    direction = np.sign(step)
    crossings = np.flatnonzero((direction*y[:-1] <= 0) & (direction*y[1:] > 0))
    if not len(crossings):
        raise ValueError(("No step crossing", int(k), flank, fraction, baseline))
    j = min(crossings, key=lambda j: abs(ii[j]-k))
    sample = ii[j] - y[j]/(y[j+1]-y[j])
    return {"time_s": float(sample/rate), "sample": float(sample),
            "step": float(step), "derivative_sample": int(k)}


def edges(x, rate, start, end, threshold, **kw):
    lo, hi = round(start*rate), round(end*rate)
    # Detect with padding: find_peaks cannot identify an endpoint derivative.
    # Apply the declared window to interpolated midpoints, not truncated samples.
    peaks, _ = signal.find_peaks(abs(np.diff(x[lo-6:hi+7])), height=threshold, distance=5)
    measured = [halfedge(x, rate, int(k), **kw) for k in peaks+lo-6 if lo-2 <= k < hi+2]
    return [e for e in measured if start <= e["time_s"] < end]


def affine(x, y):
    x, y = np.array(x), np.array(y)
    center = x.mean()
    clock = (x-center) @ (y-y.mean()) / ((x-center) @ (x-center))
    offset = np.mean(y-clock*x)
    residual = y-offset-clock*x
    sigma = np.sqrt(np.sum(residual**2)/(len(x)-2))
    return {"clock": float(clock), "offset_s": float(offset),
            "anchor_count": len(x), "center_reference_s": float(center),
            "rms_residual_T": float(np.sqrt(np.mean(residual**2))*CPU/clock),
            "max_absolute_residual_T": float(max(abs(residual))*CPU/clock),
            "ols_clock_standard_error_ppm": float(sigma/np.sqrt(np.sum((x-center)**2))*1e6),
            "ols_center_standard_error_T": float(sigma/np.sqrt(len(x))*CPU/clock),
            "residuals_T": (residual*CPU/clock).tolist()}


def load():
    manifest = json.loads((KIT / "rom/manifest.json").read_text())
    markers = list(csv.DictReader((KIT / "reference/markers.csv").open()))
    apu = list(csv.DictReader((KIT / "reference/apu.csv").open()))
    for r in apu:
        r["t"] = int(r["run_start_tick_8mhz"]) / (2*CPU)
    metrics = {name: json.loads((OUT.parent / sub / "metrics.json").read_text())
               for name, sub in [("left", "reference-comparison"), ("right", "right-channel-check")]}
    cases = []
    for case in manifest["cases"]:
        if case["group"] != "ch4-dac-phase":
            continue
        p = case["parameters"]
        c = {"id": case["id"], "name": case["name"], "polynomial": f"{p['polynomial']:02x}",
             "mode": p["mode"], "P": p["pre_disable_T"], "D": p["off_wait_T"], "R": p["post_enable_T"]}
        for label, event in [("start", "2"), ("end", "3")]:
            c[label] = next(int(r["run_start_tick_8mhz"])/(2*CPU) for r in markers
                            if int(r["case_id"]) == c["id"] and r["event"] == event)
        writes = [r for r in apu if c["start"] <= r["t"] <= c["end"]]
        c["initial_trigger"], c["retrigger"] = [r["t"] for r in writes if r["address"] == "0xff23"]
        c["initial_dac_on"] = next(r["t"] for r in writes if r["address"] == "0xff21")
        c["poweroff"] = next(r["t"] for r in apu if r["t"] > c["end"] and r["address"] == "0xff26")
        c["readbacks"] = {name: next(r["readback"] for r in m["cases"] if r["id"] == c["id"])
                          for name, m in metrics.items()}
        if c["mode"] == "dac-off":
            nr42 = [r["t"] for r in writes if r["address"] == "0xff21"]
            c["disable"], c["enable"] = nr42[1:]
            assert round((c["enable"]-c["disable"])*CPU) == c["D"]+20
            assert round((c["retrigger"]-c["enable"])*CPU) == c["R"]+48
        assert round((c["retrigger"]-c["initial_trigger"])*CPU) == 32976+c["P"]+c["D"]+c["R"]
        cases.append(c)
    rr, ref = read(KIT / "reference/audio.wav")
    hr, hw = read(HARDWARE)
    audios = {"reference": (rr, ref[:, 0]), "left": (hr, hw[:, 0]), "right": (hr, hw[:, 1])}
    return cases, audios, metrics


def measure(cases, audios, metrics, threshold=.003, flank=3, fraction=.5, baseline="linear", anchor_subset="all", packet_calibration=False, end_guard=.0002):
    kw = dict(flank=flank, fraction=fraction, baseline=baseline)
    anchors = {name: [] for name in audios}
    # Only fixed initial DAC-on edges locate the hardware map and renderer delay.
    for name, (rate, x) in audios.items():
        seed = {"offset_s": 0, "clock": 1} if name == "reference" else metrics[name]["alignment"]
        for c in cases:
            center = seed["offset_s"]+seed["clock"]*(c["initial_dac_on"]+.00065)
            lo, hi = round((center-.00012)*rate), round((center+.00012)*rate)
            k = lo+int(np.argmax(abs(np.diff(x[lo:hi]))))
            anchors[name].append(halfedge(x, rate, k, **kw))
    ref_delays = np.array([e["time_s"]-c["initial_dac_on"] for e, c in zip(anchors["reference"], cases)])
    ref_delay = float(ref_delays.mean())
    selected = [i for i,c in enumerate(cases) if anchor_subset == "all" or
                (anchor_subset == "controls" and c["mode"] == "expired-dac-on") or
                (anchor_subset == "even" and i % 2 == 0) or
                (anchor_subset == "odd" and i % 2 == 1)]
    maps = {}
    for name in ("left", "right"):
        maps[name] = affine([cases[i]["initial_dac_on"] for i in selected],
                            [anchors[name][i]["time_s"] for i in selected])
        maps[name]["anchor_case_ids"] = [cases[i]["id"] for i in selected]
        if packet_calibration:
            seed = metrics[name]["alignment"]
            maps[name] = {"clock": seed["clock"], "offset_s": seed["offset_s"]+seed["clock"]*ref_delay,
                          "method": "Existing global packet map plus independently measured renderer delay"}
    maps["reference"] = {"clock": 1., "offset_s": ref_delay}
    rows = []
    for i, c in enumerate(cases):
        row = dict(c)
        row["audio"] = {}
        for name, (rate, x) in audios.items():
            m = maps[name]
            off, clock = m["offset_s"], m["clock"]
            # The 1ms guard excludes reconnect/trigger transients, not expected noise.
            # Window ends before poweroff; observed edge counts are recorded, never prescribed.
            ee = edges(x, rate, off+clock*(c["retrigger"]+.001),
                       off+clock*(c["poweroff"]-end_guard), .04 if name == "reference" else threshold, **kw)
            for e in ee:
                e["latency_T"] = float(((e["time_s"]-off)/clock-c["retrigger"])*CPU)
            latency = np.array([e["latency_T"] for e in ee])
            first3 = latency[:3]
            row["audio"][name] = {"initial_dac_on": anchors[name][i], "edges": ee,
                "edge_count": len(ee), "first_latency_T": float(latency[0]) if len(ee) else None,
                "second_latency_T": float(latency[1]) if len(ee) > 1 else None,
                "third_latency_T": float(latency[2]) if len(ee) > 2 else None,
                "first3_latency_T": float(first3[0]+np.mean(wrap(first3-first3[0]))) if len(first3) else None,
                "first3_phase_spread_T": float(np.ptp(wrap(first3-first3[0]))) if len(first3) else None,
                "window_start_latency_T": .001*CPU,
                "window_end_latency_T": (c["poweroff"]-c["retrigger"]-end_guard)*CPU,
                "consecutive_edge_intervals_T": np.diff(latency).tolist(),
                "consecutive_interval_mod1536_T": wrap(np.diff(latency)).tolist()}
        rows.append(row)
    return rows, {"maps": maps, "reference_delay_s": ref_delay,
                  "reference_delay_T": stats(ref_delays*CPU), "sample_rates": {k:v[0] for k,v in audios.items()}}


def fit_model(rows, name, model, metric="first3_latency_T"):
    vector = MODELS[model]
    y = np.array([r["audio"][name][metric] for r in rows])
    v = np.array([sum(r[k]*a for k,a in zip(("P", "D", "R"), vector)) for r in rows])
    z = (y+v) % PERIOD
    # Minimize wrapped squared latency residual; just ONE shared offset, no free slopes.
    grid = np.arange(PERIOD)
    errors = np.mean(wrap(z[:, None]-grid[None, :])**2, axis=0)
    seed = int(np.argmin(errors))
    fit = optimize.minimize_scalar(lambda c: np.mean(wrap(z-c)**2), bounds=(seed-2, seed+2), method="bounded")
    offset = float(fit.x % PERIOD)
    residual = wrap(z-offset)
    return {"model": model, "shared_offset_mod1536_T": offset, "residual_T": stats(residual),
            "case_residuals_T": {str(r["id"]):float(v) for r,v in zip(rows, residual)}}


def analyze(rows):
    fits = []
    for name in ("reference", "left", "right"):
        for poly in ("53", "5b"):
            for mode in ("dac-off", "expired-dac-on"):
                group = [r for r in rows if r["polynomial"] == poly and r["mode"] == mode]
                for subset in (["all", "D_nonzero", "D_zero"] if mode == "dac-off" else ["all"]):
                    selected = [r for r in group if subset == "all" or (r["D"] > 0 if subset == "D_nonzero" else r["D"] == 0)]
                    for metric in ("first_latency_T", "second_latency_T", "third_latency_T", "first3_latency_T"):
                        fits.append({"audio": name, "polynomial": poly, "mode": mode, "subset": subset,
                                     "metric": metric, "fits": sorted([fit_model(selected, name, m, metric) for m in MODELS],
                                                                       key=lambda f:f["residual_T"]["rms"])})
    contrasts = []
    for name in ("reference", "left", "right"):
        for a in rows:
            for b in rows:
                if a["polynomial"] != b["polynomial"] or a["mode"] != b["mode"]:
                    continue
                changed = [k for k in ("P", "D", "R") if a[k] != b[k]]
                if len(changed) != 1 or a[changed[0]] >= b[changed[0]]:
                    continue
                aa, bb = a["audio"][name], b["audio"][name]
                d = [bb["edges"][i]["latency_T"]-aa["edges"][i]["latency_T"]
                     for i in range(min(3, len(aa["edges"]), len(bb["edges"])))]
                contrasts.append({"audio": name, "polynomial": a["polynomial"], "mode": a["mode"],
                    "axis": changed[0], "a_id": a["id"], "b_id": b["id"],
                    "delta_wait_T": b[changed[0]]-a[changed[0]],
                    "first3_delta_latencies_T": d,
                    "delta_first3_latency_T": bb["first3_latency_T"]-aa["first3_latency_T"],
                    "delta_first3_latency_wrapped_T": float(wrap(bb["first3_latency_T"]-aa["first3_latency_T"]))})
    matched = []
    for off in rows:
        if off["mode"] != "dac-off":
            continue
        on = next((r for r in rows if r["mode"] == "expired-dac-on" and all(r[k] == off[k] for k in ("polynomial", "P", "D", "R"))), None)
        if on:
            matched.append({"polynomial": off["polynomial"], "off_id": off["id"], "on_id": on["id"],
                            **{k:off[k] for k in ("P", "D", "R")},
                            "off_minus_on_first3_latency_T": {n:off["audio"][n]["first3_latency_T"]-on["audio"][n]["first3_latency_T"] for n in off["audio"]}})
    return {"model_fits": fits, "axis_contrasts": contrasts, "matched_controls": matched,
            "channel_difference_first3_T": stats([r["audio"]["left"]["first3_latency_T"]-r["audio"]["right"]["first3_latency_T"] for r in rows]),
            "edge_count_distributions": {n: {p:dict(Counter(r["audio"][n]["edge_count"] for r in rows if r["polynomial"] == p)) for p in ("53", "5b")} for n in ("reference", "left", "right")}}


def sensitivity(cases, audios, metrics, original):
    results = []
    variants = [("threshold_0.002", {"threshold": .002}), ("threshold_0.004", {"threshold": .004}),
                ("threshold_0.005", {"threshold": .005}),
                *[(f"flank_{f}", {"flank": f}) for f in (2, 4, 5)],
                ("fraction_0.4", {"fraction": .4}), ("fraction_0.6", {"fraction": .6}),
                ("constant_baseline", {"baseline": "constant"}),
                *[(f"anchors_{s}", {"anchor_subset": s}) for s in ("controls", "even", "odd")],
                ("existing_global_packet_mapping", {"packet_calibration": True}),
                ("final_guard_0.12ms", {"end_guard": .00012})]
    for label, kw in variants:
        rows, cal = measure(cases, audios, metrics, **kw)
        fits = []
        for n in ("left", "right"):
            for p in ("53", "5b"):
                for mode in ("dac-off", "expired-dac-on"):
                    group = [r for r in rows if r["polynomial"] == p and r["mode"] == mode]
                    fits.append({"audio": n, "polynomial": p, "mode": mode,
                                 "fits": sorted([fit_model(group, n, m) for m in MODELS], key=lambda f:f["residual_T"]["rms"])})
        results.append({"variant": label, "settings": kw, "calibration": cal, "model_fits": fits,
                        "first3_changes_T": {n:stats([b["audio"][n]["first3_latency_T"]-a["audio"][n]["first3_latency_T"]
                                                     for a,b in zip(original, rows)]) for n in ("left", "right")},
                        "cases": [{"id": r["id"], "audio": {n: {k:a[k] for k in
                                      ("edge_count", "first_latency_T", "first3_latency_T")} for n,a in r["audio"].items()}}
                                  for r in rows]})
    return results


def pattern_checks(rows, metrics):
    result = {}
    for name in ("reference", "left", "right"):
        result[name] = {"interval_mod1536_residual_T": stats([v for r in rows for v in r["audio"][name]["consecutive_interval_mod1536_T"]]),
                        "first3_phase_spread_T": stats([r["audio"][name]["first3_phase_spread_T"] for r in rows])}
        if name == "reference":
            continue
        polarity = np.sign(metrics[name]["alignment"]["calibration"]["gain_hardware_per_reference"])
        mismatches = []
        for r in rows:
            ref, hw = r["audio"]["reference"], r["audio"][name]
            count = min(ref["edge_count"], hw["edge_count"])
            intervals_match = np.array_equal(np.rint(np.array(ref["consecutive_edge_intervals_T"][:count-1])/PERIOD),
                                            np.rint(np.array(hw["consecutive_edge_intervals_T"][:count-1])/PERIOD))
            signs_match = np.array_equal(np.sign([e["step"] for e in ref["edges"][:count]]),
                                        polarity*np.sign([e["step"] for e in hw["edges"][:count]]))
            if not intervals_match or not signs_match:
                mismatches.append({"id": r["id"], "intervals_match": bool(intervals_match), "signs_match": bool(signs_match)})
        result[name]["common_prefix_pattern_mismatches"] = mismatches
        result[name]["readback_low_nibble_counts"] = dict(Counter(str([v & 15 for v in r["readbacks"][name]["hardware"]]) for r in rows))
        result[name]["count_mismatch_cases"] = [r["id"] for r in rows if r["audio"][name]["edge_count"] != r["audio"]["reference"]["edge_count"]]
    return result


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--out', type=Path, required=True)
    output = parser.parse_args().out.resolve()
    output.mkdir(parents=True, exist_ok=False)
    cases, audios, metrics = load()
    rows, calibration = measure(cases, audios, metrics)
    result = {"scope": "54 CH4 cases only, ew_png0d; not a whole-suite pass", "calibration": calibration,
              "measurement_parameters": {"hardware_derivative_threshold": .003, "reference_derivative_threshold": .04,
                  "baseline": "linear, flank=3; 40/50/60% crossings and alternative flanks checked separately",
                  "noise_window": "retrigger+1ms through poweroff-0.2ms, midpoint inclusion; padded peak detection",
                  "first3_latency_T": "first latency plus mean wrapped differences of edges 1-3 from edge 1; no waveform shift",
                  "fit": "wrap1536(latency + a*P + b*D + c*R - C); only C fitted, shared within width/mode/channel",
                  "primary_calibration": "one affine CPU-log to hardware map per channel, all 54 fixed initial DAC-on edges",
                  "reference_delay": "one constant from 54 reference-only initial DAC-on edges; no noise-edge calibration"},
              "cases": rows, "pattern_checks": pattern_checks(rows, metrics), **analyze(rows)}
    result["sha256"] = {str(p):hashlib.sha256(p.read_bytes()).hexdigest() for p in
                        [HARDWARE, KIT/"rom/manifest.json", KIT/"reference/audio.wav", KIT/"reference/apu.csv", KIT/"reference/markers.csv",
                         KIT/"benchmark/noise-design.md", OUT/"analyze_ch4.py",
                         OUT.parent/"reference-comparison/metrics.json", OUT.parent/"right-channel-check/metrics.json"]}
    (output/"sensitivity.json").write_text(json.dumps(sensitivity(cases, audios, metrics, rows), indent=2, allow_nan=False)+"\n")
    (output/"measurements.json").write_text(json.dumps(result, indent=2, allow_nan=False)+"\n")
    flat = [{**{k:r[k] for k in ("id", "name", "polynomial", "mode", "P", "D", "R")}, "audio":n,
             **{k:a[k] for k in ("edge_count", "first_latency_T", "first3_latency_T", "first3_phase_spread_T")}}
            for r in rows for n,a in r["audio"].items()]
    with (output/"case-measurements.csv").open("w") as f:
        w = csv.DictWriter(f, fieldnames=list(flat[0])); w.writeheader(); w.writerows(flat)
    edge_rows = [{"case_id":r["id"], "audio":n, "edge_index":i, **e}
                 for r in rows for n,a in r["audio"].items() for i,e in enumerate(a["edges"])]
    with (output/"edges.csv").open("w") as f:
        w = csv.DictWriter(f, fieldnames=list(edge_rows[0])); w.writeheader(); w.writerows(edge_rows)
    print(json.dumps({"counts": result["edge_count_distributions"], "pattern_checks": result["pattern_checks"]}, indent=2))
    for f in result["model_fits"]:
        if f["audio"] != "reference" and f["metric"] == "first3_latency_T":
            print(f["audio"], f["polynomial"], f["mode"], f["subset"], [(a["model"], round(a["residual_T"]["rms"], 2)) for a in f["fits"]])


if __name__ == "__main__":
    main()
