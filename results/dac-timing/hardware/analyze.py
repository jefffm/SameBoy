#!/usr/bin/env python3
"""Inspect recorded mixed-DAC controls, retaining original packet timing/gain.

No parameter is fitted. Native strongest signed derivative identifies edge time;
windows come from the 64-tap synthesis support. The calibration's256Hz period
supplies independent, complete-cycle tone-edge windows. Amplitude means use the
kernel-derived windows on either side of the transition, as descriptive levels, not fitted
step heights. Original WAVs are unchanged.
"""
import csv,json,sys,warnings
from pathlib import Path
import numpy as np
import matplotlib
matplotlib.use('Agg')
import matplotlib.pyplot as plt
from scipy.io import wavfile
ROOT=Path(__file__).resolve().parents[3]
sys.path.insert(0,str(ROOT/'oracle-analysis'))
from mute_tail_probe import read_channel,digest
out=Path(__file__).resolve().parent
mp=ROOT/'artifacts/torture/comparison-torture-pula2a6h/metrics.json';m=json.loads(mp.read_text())
render=ROOT/'artifacts/dac-on-edge/torture-after';prov=json.loads((render/'provenance.json').read_text())
assert prov['status']=='completed'
for k in ['hardware','reference','manifest','markers']: assert digest(ROOT/m[k]['path'])==m[k]['sha256']
for n,hsh in prov['outputs_sha256'].items():assert digest(render/n)==hsh
assert digest(render/'markers.csv')==m['markers']['sha256']
tr=list(csv.DictReader((render/'apu.csv').open()))
clock=m['alignment']['clock'];offset=m['alignment']['offset_s'];gain=m['alignment']['calibration']['gain_hardware_per_reference']
toh=lambda t:offset+clock*t;toe=lambda t:(t-offset)/clock
warnings.filterwarnings('ignore',category=wavfile.WavFileWarning)
r,e=read_channel(render/'audio.wav',0);e*=gain;hr,h=read_channel(ROOT/m['hardware']['path'],0)
width=64/r;delay=32/r
diagnostic=ROOT/'artifacts/dac-connected/final-mask-render'
dp=json.loads((diagnostic/'provenance.json').read_text());assert dp['status']=='completed'
for prior in ['delay-render', 'final-render']:
 prior_dir=ROOT/'artifacts/dac-connected'/prior
 prior_prov=json.loads((prior_dir/'provenance.json').read_text())
 assert prior_prov['status']=='completed' and prior_prov['outputs_sha256']==dp['outputs_sha256']
 for n,hsh in prior_prov['outputs_sha256'].items():assert digest(prior_dir/n)==hsh
for n,hsh in dp['outputs_sha256'].items():assert digest(diagnostic/n)==hsh
assert digest(diagnostic/'markers.csv')==m['markers']['sha256']
assert digest(diagnostic/'apu.csv')==digest(render/'apu.csv')
dr,de=read_channel(diagnostic/'audio.wav',0);de*=gain
sources={'emulator':(r,e),'delayed_fade':(dr,de),'hardware':(hr,h)}

def native(name,a,b):
 rate,x=sources[name];lo,hi=(toh(a),toh(b)) if name=='hardware' else (a,b)
 i,j=int(np.ceil(lo*rate)),int(np.floor(hi*rate));ts=np.arange(i,j)/rate
 return (toe(ts) if name=='hardware' else ts),x[i:j],(i,j)

def measured(name,t,sign):
 ts,x,(i,j)=native(name,t-delay,t+width)
 dx=np.diff(x);k=int(np.argmax(dx) if sign>0 else np.argmin(dx));edge=float((ts[k]+ts[k+1])/2)
 # Derivative peak spans two native samples; midpoint is the estimator.
 before=x[ts<t-delay/2];after=x[ts>t+delay*1.5]
 return {'native_sample_interval':[i,j],'strongest_derivative_time_s':edge,
         'derivative_interval_s':[float(ts[k]),float(ts[k+1])],
         'edge_offset_ms':(edge-t)*1000,'strongest_delta':float(dx[k]),
         'min':float(min(x)),'max':float(max(x)),
         'strongest_absolute_derivative_time_s':float((ts[np.argmax(abs(dx))]+ts[np.argmax(abs(dx))+1])/2),
         'strongest_absolute_derivative_delta':float(dx[np.argmax(abs(dx))]),
         'early_mean':float(np.mean(before)),'late_mean':float(np.mean(after)),
         'early_rms':float(np.sqrt(np.mean(before*before))), 'late_rms':float(np.sqrt(np.mean(after*after)))}

cases=[];plotted=[]
figs={}
for name in ['controls','alloff']:
 figs[name]=plt.subplots(4,3,figsize=(14,12),layout='constrained')
for ident in range(177,185):
 row=(ident-177)//2
 fig,axs=figs['controls' if ident%2==0 else 'alloff']
 case=next(c for c in m['cases'] if c['id']==ident);a,b=case['reference_interval_s'];rows=[v for v in tr if a<=float(v['run_start_seconds'])<=b]
 channel=row+1;reg={1:'0xff12',2:'0xff17',3:'0xff1a',4:'0xff21'}[channel]
 on=next(v for v in reversed(rows) if v['address']==reg and int(v['value'],16))
 off=next(v for v in reversed(rows[:rows.index(on)]) if v['address']==reg and int(v['value'],16)==0)
 if channel==3:
  # Redundant second DAC-off in waveform setup is not the measured off edge.
  off=next(v for v in rows if v['address']==reg and v['value']=='0x00' and float(v['run_start_seconds'])>a+width)
  zero=next(v for v in rows if v['address']=='0xff1c' and v['value']=='0x00')
 else:
  zero=next(v for v in rows if v['address']==reg and v['value']=='0x08')
 events=[]
 for col,(kind,event,sign) in enumerate([('digital-zero',zero,+1),('DAC-off',off,-1),('DAC-on',on,+1)]):
  t=float(event['run_start_seconds']);measure={n:measured(n,t,sign) for n in sources}
  events.append({'kind':kind,'write':event,'edge_sign_requested':sign,'measurements':measure,
                 'hardware_minus_emulator_edge_samples':(measure['hardware']['strongest_derivative_time_s']-measure['emulator']['strongest_derivative_time_s'])*r,
                 'hardware_minus_delayed_edge_samples':(measure['hardware']['strongest_derivative_time_s']-measure['delayed_fade']['strongest_derivative_time_s'])*r})
  ax=axs[row,col]
  for n in sources:
   ts,x,indices=native(n,t-delay,t+width);ax.plot((ts-t)*1000,x*1000,label=n,lw=1,marker='.',ms=2)
   plotted.extend((ident,kind,n,indices[0]+k,float(tx),float(v)) for k,(tx,v) in enumerate(zip(ts,x)))
  ax.axvline(0,color='#777',ls=':',lw=.8);ax.axvline(delay*1000,color='#777',ls='--',lw=.8)
  ax.set_title(f'CH{channel}: {kind}');ax.grid(alpha=.2);ax.set_xlim(-delay*1000,width*1000);ax.set_ylabel('Recording amplitude ×1000');ax.set_xlabel('ms from CPU write')
 cases.append({'id':ident,'name':case['name'],'channel':channel,'case_interval_s':[a,b],'writes':rows,'events':events})
for name,(fig,axs) in figs.items():
 axs[0,0].legend();fig.suptitle(('Other DAC remains enabled' if name=='controls' else 'All DACs disabled')+': digital changes versus DAC envelope edges',fontsize=14)
 fig.text(.5,-.022,'Dotted: register write. Dashed: 32 native samples, derived from64-tap synthesis kernel. Original packet map and gain; no local fitting.',ha='center',fontsize=9)
 for ext in ['png','pdf']:fig.savefig(out/f'edge-{name}.{ext}',dpi=170,bbox_inches='tight')
 plt.close(fig)
with (out/'edge-controls-native.csv').open('w') as f:
 w=csv.writer(f);w.writerow(['case','event','source','frame','mapped_time_s','recording_amplitude']);w.writerows(plotted)
# Independent calibration tone: measure all complete source-defined periods,
# retaining both derivative signs. No shift is estimated or applied.
cal=next(c for c in m['cases'] if c['id']==1);start,end=cal['reference_interval_s'];period=1/256;tones=[]
for k in range(int((end-start)/period)):
 a,b=start+k*period,start+(k+1)*period
 for sign in [-1,1]:
  v={}
  for n in sources:
   ts,x,inds=native(n,a,b);d=np.diff(x);i=int(np.argmax(d) if sign>0 else np.argmin(d));v[n]={'native_frame_pair':[inds[0]+i,inds[0]+i+1],'edge_time_s':float((ts[i]+ts[i+1])/2),'delta':float(d[i])}
  tones.append({'period':k,'sign':sign,'sources':v,'hardware_minus_emulator_samples':(v['hardware']['edge_time_s']-v['emulator']['edge_time_s'])*r})
# Keep every packet timing residual already measured, plus local marker offsets.
packet_controls=[]
for ident in [1,178,180,182,184]:
 rp=next(p for p in m['reference_packets'] if p['kind']==1 and p['id']==ident)
 hp=next(p for p in m['hardware_packets'] if p['kind']==1 and p['id']==ident)
 packet_controls.append({'case':ident,'reference_start_s':rp['start_s'],'hardware_start_s':hp['start_s'],'frozen_map_residual_samples':(toe(hp['start_s'])-rp['start_s'])*r})
inputs={str(p.relative_to(ROOT)):digest(p) for p in [mp,render/'audio.wav',render/'apu.csv',render/'markers.csv',render/'provenance.json',diagnostic/'audio.wav',diagnostic/'apu.csv',diagnostic/'provenance.json']};inputs[m['hardware']['path']]=digest(ROOT/m['hardware']['path'])
steady_tone=[v['hardware_minus_emulator_samples'] for v in tones if v['period']>0]
result={'diagnostic_final_outputs_identical':True,'steady_tone_summary':{'periods':len(steady_tone)//2,'edge_count':len(steady_tone),'hardware_minus_emulator_samples_range':[min(steady_tone),max(steady_tone)],'rms_samples':float(np.sqrt(np.mean(np.array(steady_tone)**2))),'selection_authority':'The initial source-period contains trigger/startup transients; all remaining complete256Hz periods are used. Initial-period observations remain in calibration_tone_edges.'},'method':__doc__,'inputs_sha256':inputs,'frozen_mapping':{'clock':clock,'offset_s':offset,'gain':gain},'synthesis':{'width_samples':64,'midpoint_samples':32,'rate':r},'cases':cases,'calibration_tone_edges':tones,'packet_controls':packet_controls,'original_inputs_unchanged':all(digest(ROOT/p)==v for p,v in inputs.items()),'limitations':'Tone calibration and packet map already align band-limited audio;32-sample register offset is not a physical latency measurement. Strongest signed derivative can select a later oscillator edge when the immediate event is masked; raw waveform and writes retained. No clipping, gain adjustment, or per-variant timing adjustment.'}
(out/'analysis.json').write_text(json.dumps(result,indent=2)+'\n')
print(json.dumps({'events':[{'id':c['id'],'edges':[{k:v for k,v in e.items() if k!='write'} for e in c['events']]}for c in cases],'tone_delta_samples':[v['hardware_minus_emulator_samples'] for v in tones],'packet_controls':packet_controls},indent=2))
