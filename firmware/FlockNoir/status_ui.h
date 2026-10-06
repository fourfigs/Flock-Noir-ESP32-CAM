#pragma once
#include <Arduino.h>

// Browser presentation only; /api/status?format=json retains the JSON API.
static const char STATUS_HTML[] PROGMEM = R"STATUS(
<!doctype html><html lang="en"><head>
<meta charset="utf-8"><meta name="viewport" content="width=device-width,initial-scale=1">
<title>System status | Flock Noir</title>
<style>
:root{color-scheme:dark;--bg:#080e16;--panel:#111c29;--line:#263647;--text:#e9f1fa;--muted:#9caec2;--good:#66e5ac;--warn:#ffd16b;--bad:#ff8590;--off:#80baff}
*{box-sizing:border-box}body{margin:0;background:radial-gradient(ellipse at top right,#153047,transparent 60%),var(--bg);color:var(--text);font:15px system-ui,sans-serif;line-height:1.5}main{max-width:1100px;margin:auto;padding:36px 22px}a{color:var(--off)}header{display:flex;align-items:center;justify-content:space-between;gap:20px;flex-wrap:wrap;margin-bottom:26px}h1{font-size:32px;letter-spacing:-1px;margin:2px 0}h2{font-size:17px;margin:0}.eyebrow{color:var(--muted);letter-spacing:2px;font-size:11px;text-transform:uppercase}.sub{color:var(--muted);margin:6px 0}.legend{display:flex;gap:16px;flex-wrap:wrap;margin:20px 0}.good{--state:var(--good)}.warn{--state:var(--warn)}.bad{--state:var(--bad)}.off{--state:var(--off)}.badge{color:var(--state);font-size:12px;font-weight:650}.badge:before{content:'●';margin-right:7px}.grid{display:grid;grid-template-columns:repeat(auto-fit,minmax(285px,1fr));gap:16px}.card{border:1px solid var(--line);border-top:3px solid var(--state);background:var(--panel);border-radius:14px;padding:20px}.cardhead{display:flex;justify-content:space-between;align-items:center;gap:12px}.note{color:var(--muted);font-size:13px;margin:10px 0 16px}dl{margin:0}dl>div{display:flex;justify-content:space-between;gap:15px;padding:8px 0;border-top:1px solid var(--line)}dt{color:var(--muted)}dd{margin:0;text-align:right;font-variant-numeric:tabular-nums;overflow-wrap:anywhere}details{margin-top:24px;border:1px solid var(--line);border-radius:14px;padding:18px;background:var(--panel)}summary{cursor:pointer}pre{white-space:pre-wrap;overflow-wrap:anywhere;color:var(--muted);font-size:12px}footer{margin-top:22px;color:var(--muted);font-size:12px}#connection{padding:8px 12px;border:1px solid var(--line);border-radius:20px}
</style></head><body><main>
<header><div><div class="eyebrow">Flock Noir · device diagnostics</div><h1>System status</h1><p class="sub">Camera, modules, and runtime health at a glance.</p></div><a href="/">← Dashboard</a></header>
<div id="connection" class="warn badge" role="status">Connecting…</div>
<div class="legend"><span class="good badge">Healthy</span><span class="warn badge">Attention / OK</span><span class="bad badge">Failure</span><span class="off badge">Off / inactive</span></div>
<section id="cards" class="grid" aria-label="Module status"></section>
<details><summary>Complete status data · <a href="/api/status?format=json">JSON</a></summary><pre id="raw">Waiting for device…</pre></details>
<footer>Refreshes every 2 seconds. Status reflects firmware reports, not proof of physical wiring. Error counters are cumulative.</footer>
</main><script>
const gpsAssigned=__GPS_ENABLED__,buzzerAssigned=__BUZZER_ENABLED__;
const esc=v=>String(v??'—').replace(/[&<>"']/g,c=>({'&':'&amp;','<':'&lt;','>':'&gt;','"':'&quot;',"'":'&#39;'}[c]));
const num=v=>Number.isFinite(Number(v))?Number(v).toLocaleString():'—';
function card(title,state,label,note,rows){return `<article class="card ${state}"><div class="cardhead"><h2>${esc(title)}</h2><span class="badge">${esc(label)}</span></div><p class="note">${esc(note)}</p><dl>${rows.map(([k,v])=>`<div><dt>${esc(k)}</dt><dd>${esc(v)}</dd></div>`).join('')}</dl></article>`;}
function render(s){
 const cameraState=!s.cameraReady?'bad':(!s.cameraFrames||s.captureFps<=0||s.cameraDecodeErrors)?'warn':'good';
 const analysisState=!s.opticalActive?'off':!s.cameraReady?'bad':(!s.analysisFrames||s.fps<=0)?'warn':'good';
 const cards=[
 card('Camera',cameraState,!s.cameraReady?'Unavailable':cameraState==='warn'?'Check capture':'Ready','Capture rate is the latest estimate; rejected-frame and decode errors share one counter.',[['Resolution',`${s.cameraWidth} × ${s.cameraHeight}`],['Capture rate',`${s.captureFps} FPS`],['Frames received',num(s.cameraFrames)],['Frame / decode errors',num(s.cameraDecodeErrors)]]),
 card('Optical analysis',analysisState,!s.opticalActive?'Inactive':analysisState==='good'?'Running':analysisState==='bad'?'Unavailable':'Waiting','Optical analysis runs only in the ALPR profile. A candidate is not a confirmed identification.',[['Profile',s.profile],['Analysis rate',`${s.fps} FPS`],['Frames analyzed',num(s.analysisFrames)],['Decode time',`${s.cameraAnalysisMs} ms`],['Candidate',s.detected?'Detected':'None'],['Confidence',`${(Number(s.confidence)*100).toFixed(1)}%`],['Frequency',`${s.freq} Hz`],['Duty cycle',`${(Number(s.duty)*100).toFixed(1)}%`]]),
 card('GPS',!gpsAssigned?'off':s.fix?'good':'warn',!gpsAssigned?'Disabled':s.fix?'Fix acquired':'No fix',!gpsAssigned?'GPS pins are unassigned on this build.':'No fix can mean no module, no signal, or a pending acquisition.',[['Satellites',num(s.sats)],['Characters',num(s.gpsChars)],['Valid sentences',num(s.gpsGood)],['Checksum failures',num(s.gpsFail)],['Time',s.time]]),
 card('OPT101',!s.irPresent||!s.irEn||s.irPaused?'off':s.irClipped?'warn':'good',!s.irPresent?'Unavailable':!s.irEn?'Disabled':s.irPaused?'Paused':s.irClipped?'Clipped':'Active','Presence reflects whether the sampling task started.',[['Enabled',s.irEn?'Yes':'No'],['Paused',s.irPaused?'Yes':'No'],['Sample rate',`${s.irSampleHz} Hz`],['Sampling gaps',num(s.irGaps)],['Dropped events',num(s.irDroppedEvents)]]),
 card('Storage & recording',s.logErrors||s.wdErrors?'warn':s.sd?'good':'off',s.logErrors||s.wdErrors?'Check errors':s.sd?'Mounted':'No SD','An absent or unmounted SD card does not prevent the dashboard from running.',[['SD free / total',`${s.sdFree} / ${s.sdTotal} MB`],['Recording',s.rec?'Active':'Off'],['Audio recording',s.recAudio?'Active':'Off'],['Recorded frames',num(s.recFrames)],['Log errors',num(s.logErrors)],['Wardrive errors',num(s.wdErrors)]]),
 card('Radio & wardrive',s.wd?'good':'off',s.wd?'Wardrive on':'Wardrive off','Radio event counts report observations; they do not prove radio hardware health.',[['Scan profile',s.profile],['Wardrive scanning',s.wdScan?'Yes':'No'],['Radio events',num(s.radioEvents)],['Wi-Fi logged',num(s.wdWifiLogged)],['BLE logged',num(s.wdBleLogged)],['Skipped without fix',num(s.wdNoFix)]]),
 card('Buzzer',!buzzerAssigned||!s.buzzer||s.muted?'off':'warn',!buzzerAssigned?'Disabled':s.muted?'Muted':s.buzzer?'Enabled':'Off',!buzzerAssigned?'Buzzer pin is unassigned on this build.':'Enabled is a software setting; physical output is not verified.',[['Setting',s.buzzer?'Enabled':'Off'],['Muted',s.muted?'Yes':'No']]),
 card('System','good','Responding','This page is receiving status from the device.',[['Firmware',s.version],['Uptime',`${num(s.uptime)} seconds`],['Alerts logged',num(s.logged)]])
 ];document.getElementById('cards').innerHTML=cards.join('');document.getElementById('raw').textContent=JSON.stringify(s,null,2);
}
async function poll(){const el=document.getElementById('connection');try{const r=await fetch('/api/status?format=json',{cache:'no-store',signal:AbortSignal.timeout(5000)});if(!r.ok)throw Error(r.status);render(await r.json());el.className='good badge';el.textContent='Live · updated '+new Date().toLocaleTimeString();}catch(e){el.className='bad badge';el.textContent='Connection failed · displayed data may be stale';}finally{setTimeout(poll,2000);}}poll();
</script></body></html>
)STATUS";
