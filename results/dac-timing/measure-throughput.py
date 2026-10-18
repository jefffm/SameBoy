import hashlib,json,resource,subprocess,tempfile,time
from pathlib import Path
root=Path.cwd();out=root/'artifacts/dac-connected'; results=[]
manifest=json.loads((root/'artifacts/torture/renders/render-h1xqcgiz/manifest.json').read_text())
def sha(p):
 with open(p,'rb') as f:return hashlib.file_digest(f,'sha256').hexdigest()
for name,render in [('before','artifacts/dac-on-edge/torture-after'),('after','artifacts/dac-connected/final-mask-render')]:
 render=root/render
 with tempfile.TemporaryDirectory(prefix=f'perf-{name}-',dir=out) as tmp:
  tmp=Path(tmp)
  cmd=[str(render/'capture'),'--rom',manifest['rom'],'--boot',str(root/'SameBoy/build/bin/BootROMs/dmg_boot.bin'),'--until-done','--seed','1','--wav',str(tmp/'audio.wav'),'--screenshot',str(tmp/'screen.ppm'),'--apu-log',str(tmp/'apu.csv'),'--markers-log',str(tmp/'markers.csv')]
  usage0=resource.getrusage(resource.RUSAGE_CHILDREN); start=time.perf_counter()
  with (out/f'perf-{name}.log').open('w') as log:subprocess.run(cmd,stdout=log,stderr=subprocess.STDOUT,check=True)
  elapsed=time.perf_counter()-start;usage1=resource.getrusage(resource.RUSAGE_CHILDREN)
  hashes={n:sha(tmp/n) for n in ['audio.wav','screen.ppm','apu.csv','markers.csv']}
  original=json.loads((render/'provenance.json').read_text())
  assert hashes==original['outputs_sha256']
  results.append({'variant':name,'wall_seconds':elapsed,'user_cpu_seconds':usage1.ru_utime-usage0.ru_utime,'system_cpu_seconds':usage1.ru_stime-usage0.ru_stime,'capture_sha256':sha(render/'capture'),'provenance_path':str(render/'provenance.json'),'command':cmd,'outputs_sha256':hashes,'outputs_identical_to_validated_render':True})
  print(name,elapsed,flush=True)
report={'method':'One sequential before/after render of the same finite hardware-suite ROM at48kHz, same seed and I/O. ROM suite-end terminates each run. These single observations do not estimate a small regression or speedup statistically. Temporary duplicate outputs were verified against the retained original render hashes before removal.','rendered_audio_seconds':563.6452291666667,'results':results}
(out/'throughput.json').write_text(json.dumps(report,indent=2)+'\n')
