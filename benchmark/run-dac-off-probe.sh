#!/bin/sh
set -eu
cd "$(dirname "$0")/.."
capture=${1:-benchmark/capture}
prefix=${2:-artifacts/noise-probe/gate-hold}
sameboy=${3:-SameBoy}
mkdir -p artifacts/noise-probe
for mode in dac-off dac-control; do
    keep_dac=0
    if [ "$mode" = dac-control ]; then keep_dac=1; fi
    rgbasm -D "KEEP_DAC=$keep_dac" -o "artifacts/noise-probe/$mode.o" benchmark/dac-off-probe.asm
    rgblink -o "artifacts/noise-probe/$mode.gb" "artifacts/noise-probe/$mode.o"
    rgbfix -v -p 0 -t 'DAC OFF PROBE' "artifacts/noise-probe/$mode.gb"
    "$capture" --rom "artifacts/noise-probe/$mode.gb" \
        --boot "$sameboy/build/bin/BootROMs/dmg_boot.bin" --seconds 18.3 --seed 1 \
        --wav "$prefix-$mode.wav" --screenshot "$prefix-$mode.ppm" \
        --apu-log "$prefix-$mode.csv"
done
