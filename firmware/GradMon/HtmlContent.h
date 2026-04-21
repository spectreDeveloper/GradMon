#pragma once

// ── Main web UI (stored in flash, not RAM) ────────────────────────────────────
const char HTML_INDEX[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="it">
<head>
<meta charset="UTF-8">
<meta name="viewport" content="width=device-width,initial-scale=1">
<title>GradMon</title>
<style>
*{box-sizing:border-box;margin:0;padding:0}
body{background:#0f0f0f;color:#e8e8e8;font-family:-apple-system,sans-serif;max-width:480px;margin:0 auto;padding:16px 12px}
h1{text-align:center;font-size:26px;color:#f5c518;margin-bottom:20px;letter-spacing:2px}
.card{background:#1a1a1a;border-radius:12px;padding:16px;margin-bottom:14px;border:1px solid #2a2a2a}
h2{font-size:11px;color:#777;text-transform:uppercase;letter-spacing:1.5px;margin-bottom:12px}
input,select{width:100%;background:#252525;border:1px solid #383838;color:#e8e8e8;border-radius:8px;padding:10px 12px;font-size:14px;margin-bottom:10px;outline:none}
input:focus,select:focus{border-color:#f5c518}
.btn{width:100%;background:#f5c518;color:#0f0f0f;border:none;border-radius:8px;padding:12px;font-size:14px;font-weight:700;cursor:pointer;margin-top:4px}
.btn:hover{background:#e0b010}
.btn-sec{background:#252525;color:#e8e8e8}
.btn-sec:hover{background:#333}
.result-item{background:#252525;border-radius:8px;padding:10px;margin-bottom:8px;cursor:pointer;display:flex;align-items:center;gap:10px;border:1px solid transparent;transition:.15s}
.result-item:hover{border-color:#f5c518}
.result-item.sel{border-color:#f5c518;background:#2a2600}
.result-img{width:38px;height:52px;object-fit:contain;border-radius:4px;background:#1a1a1a}
.result-name{font-size:13px;font-weight:600;line-height:1.3}
.result-set{font-size:11px;color:#888;margin-top:2px}
.grade-row{display:grid;grid-template-columns:1fr 1fr;gap:8px}
.grade-row select{margin-bottom:0}
.price-big{font-size:28px;color:#f5c518;text-align:center;margin:12px 0 4px;font-weight:700}
.status{font-size:12px;color:#888;text-align:center;margin-top:8px;min-height:16px}
.ok{color:#4caf50}
.err{color:#f44}
/* OLED mock: 128×64 proportional */
.oled-frame{background:#000;border:2px solid #2a2a2a;border-radius:6px;padding:10px 12px;font-family:'Courier New',monospace;font-size:12px;line-height:1.55;color:#c8e6c9}
.oled-hr{border:none;border-top:1px solid #1a3a1a;margin:4px 0}
.oled-lg{font-size:14px;font-weight:700;color:#81c784}
.hidden{display:none}
</style>
</head>
<body>
<h1>&#9673; GRADMON</h1>

<!-- API Config -->
<div class="card">
  <h2>Configurazione API</h2>
  <input type="password" id="apiKey" placeholder="TCGgo API Key" autocomplete="new-password">
  <select id="currency">
    <option value="USD">USD ($)</option>
    <option value="EUR">EUR (€)</option>
    <option value="GBP">GBP (£)</option>
    <option value="JPY">JPY (¥)</option>
    <option value="CAD">CAD (CA$)</option>
  </select>
  <button class="btn" onclick="saveConfig()">Salva</button>
  <p class="status" id="cfgStatus"></p>
</div>

<!-- Search -->
<div class="card">
  <h2>Cerca Carta</h2>
  <input type="text" id="searchQ" placeholder="es. Charizard Base Set 1999…" onkeydown="if(event.key==='Enter')searchCards()">
  <button class="btn btn-sec" onclick="searchCards()">Cerca</button>
  <div id="results"></div>
</div>

<!-- Grade + price (hidden until card selected) -->
<div class="card hidden" id="gradeCard">
  <h2>Grado &amp; Prezzo</h2>
  <div class="grade-row">
    <select id="grader" onchange="clearPrice()">
      <option>PSA</option><option>BGS</option><option>SGC</option><option>CGC</option>
    </select>
    <select id="grade" onchange="clearPrice()">
      <option>10</option><option>9.5</option><option>9</option>
      <option>8.5</option><option>8</option><option>7.5</option>
      <option>7</option><option>6</option><option>5</option>
      <option>4</option><option>3</option><option>2</option><option>1</option>
    </select>
  </div>
  <button class="btn btn-sec" style="margin-top:10px" onclick="getPrice()">Ottieni Prezzo</button>
  <div class="price-big hidden" id="priceBig"></div>
  <button class="btn hidden" id="sendBtn" onclick="sendToDisplay()">&#9654; Invia al Display OLED</button>
  <p class="status" id="gradeStatus"></p>
</div>

<!-- OLED preview -->
<div class="card">
  <h2>Preview Display OLED (128×64)</h2>
  <div class="oled-frame" id="oledPreview">[nessuna carta selezionata]</div>
</div>

<script>
var sel = null, selPrice = null;

function sym(c){return{USD:'$',EUR:'€',GBP:'£',JPY:'¥',CAD:'CA$'}[c]||c+' ';}
function fmtPrice(v,c){return sym(c)+parseFloat(v).toLocaleString(undefined,{minimumFractionDigits:2,maximumFractionDigits:2});}
function trunc(s,n){return s&&s.length>n?s.slice(0,n-1)+'~':s||'';}

async function saveConfig(){
  var k=document.getElementById('apiKey').value.trim();
  var c=document.getElementById('currency').value;
  if(!k){setStatus('cfgStatus','Inserisci la API key','err');return;}
  var r=await fetch('/api/config',{method:'POST',headers:{'Content-Type':'application/json'},body:JSON.stringify({apiKey:k,currency:c})});
  var d=await r.json();
  document.getElementById('apiKey').value='';
  setStatus('cfgStatus',d.ok?'Configurazione salvata!':'Errore: '+d.error,d.ok?'ok':'err');
}

async function searchCards(){
  var q=document.getElementById('searchQ').value.trim();
  if(!q)return;
  document.getElementById('results').innerHTML='<p class="status">Ricerca in corso…</p>';
  var r=await fetch('/api/search?q='+encodeURIComponent(q));
  var d=await r.json();
  if(!d.ok){document.getElementById('results').innerHTML='<p class="status err">'+d.error+'</p>';return;}
  var cards=d.results||[];
  if(!cards.length){document.getElementById('results').innerHTML='<p class="status">Nessun risultato</p>';return;}
  var html='';
  cards.forEach(function(c,i){
    html+='<div class="result-item" id="ri'+i+'" onclick="pick('+i+','+JSON.stringify(JSON.stringify(c))+')">';
    if(c.imageUrl)html+='<img class="result-img" src="'+c.imageUrl+'" loading="lazy" onerror="this.style.display=\'none\'">';
    else html+='<div class="result-img"></div>';
    html+='<div><div class="result-name">'+c.name+'</div><div class="result-set">'+c.setName+' '+(c.year||'')+'</div></div></div>';
  });
  document.getElementById('results').innerHTML=html;
}

function pick(i,raw){
  document.querySelectorAll('.result-item').forEach(function(e){e.classList.remove('sel');});
  document.getElementById('ri'+i).classList.add('sel');
  sel=JSON.parse(raw);
  selPrice=null;
  document.getElementById('gradeCard').classList.remove('hidden');
  document.getElementById('priceBig').classList.add('hidden');
  document.getElementById('sendBtn').classList.add('hidden');
  document.getElementById('gradeStatus').textContent='';
  updatePreview();
}

function clearPrice(){selPrice=null;document.getElementById('priceBig').classList.add('hidden');document.getElementById('sendBtn').classList.add('hidden');}

async function getPrice(){
  if(!sel)return;
  var grader=document.getElementById('grader').value;
  var grade=document.getElementById('grade').value;
  var currency=document.getElementById('currency').value;
  setStatus('gradeStatus','Recupero prezzo…','');
  var r=await fetch('/api/price',{method:'POST',headers:{'Content-Type':'application/json'},body:JSON.stringify({cardId:sel.id,grader:grader,grade:grade,currency:currency})});
  var d=await r.json();
  if(d.ok){
    selPrice={value:d.price,currency:currency};
    var el=document.getElementById('priceBig');
    el.textContent=fmtPrice(d.price,currency);
    el.classList.remove('hidden');
    document.getElementById('sendBtn').classList.remove('hidden');
    setStatus('gradeStatus','','');
    updatePreview();
  } else {
    setStatus('gradeStatus','Errore: '+d.error,'err');
  }
}

async function sendToDisplay(){
  if(!sel)return;
  var grader=document.getElementById('grader').value;
  var grade=document.getElementById('grade').value;
  var currency=selPrice?selPrice.currency:document.getElementById('currency').value;
  var price=selPrice?fmtPrice(selPrice.value,currency):'---';
  var payload={name:sel.name,setName:sel.setName||'',grade:grader+' '+grade,price:price};
  var r=await fetch('/api/display',{method:'POST',headers:{'Content-Type':'application/json'},body:JSON.stringify(payload)});
  var d=await r.json();
  setStatus('gradeStatus',d.ok?'Inviato al display!':'Errore: '+d.error,d.ok?'ok':'err');
}

function updatePreview(){
  if(!sel){document.getElementById('oledPreview').innerHTML='[nessuna carta selezionata]';return;}
  var grader=document.getElementById('grader').value;
  var grade=document.getElementById('grade').value;
  var n=trunc(sel.name,21);
  var s=trunc(sel.setName||'',21);
  var g=trunc(grader+' '+grade,18);
  var p=selPrice?trunc(fmtPrice(selPrice.value,selPrice.currency),18):'---';
  document.getElementById('oledPreview').innerHTML=
    n+'<br>'+s+'<hr class="oled-hr"><span class="oled-lg">'+g+'<br>'+p+'</span>';
}

function setStatus(id,msg,cls){
  var el=document.getElementById(id);
  el.textContent=msg;
  el.className='status '+(cls||'');
}

// Load saved currency on startup
fetch('/api/status').then(function(r){return r.json();}).then(function(d){
  if(d.currency)document.getElementById('currency').value=d.currency;
  if(d.cardName){
    document.getElementById('oledPreview').innerHTML=
      trunc(d.cardName,21)+'<br>'+trunc(d.cardSet||'',21)+
      '<hr class="oled-hr"><span class="oled-lg">'+trunc(d.cardGrade||'',18)+'<br>'+trunc(d.cardPrice||'---',18)+'</span>';
  }
}).catch(function(){});
</script>
</body>
</html>
)rawliteral";

// ── WiFi setup page (served in AP mode) ───────────────────────────────────────
const char HTML_WIFI_SETUP[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="it">
<head>
<meta charset="UTF-8">
<meta name="viewport" content="width=device-width,initial-scale=1">
<title>GradMon — Setup WiFi</title>
<style>
*{box-sizing:border-box;margin:0;padding:0}
body{background:#0f0f0f;color:#e8e8e8;font-family:-apple-system,sans-serif;max-width:360px;margin:60px auto;padding:0 16px}
h1{text-align:center;font-size:24px;color:#f5c518;margin-bottom:8px}
p{text-align:center;color:#888;font-size:13px;margin-bottom:24px}
.card{background:#1a1a1a;border-radius:12px;padding:20px;border:1px solid #2a2a2a}
input{width:100%;background:#252525;border:1px solid #383838;color:#e8e8e8;border-radius:8px;padding:10px 12px;font-size:14px;margin-bottom:12px;outline:none}
input:focus{border-color:#f5c518}
button{width:100%;background:#f5c518;color:#0f0f0f;border:none;border-radius:8px;padding:12px;font-size:14px;font-weight:700;cursor:pointer}
.status{font-size:12px;color:#888;text-align:center;margin-top:10px}
</style>
</head>
<body>
<h1>&#9673; GradMon</h1>
<p>Configura la connessione WiFi</p>
<div class="card">
  <input type="text" id="ssid" placeholder="Nome rete WiFi (SSID)" autocomplete="off">
  <input type="password" id="pass" placeholder="Password WiFi" autocomplete="new-password">
  <button onclick="save()">Connetti</button>
  <p class="status" id="st"></p>
</div>
<script>
async function save(){
  var s=document.getElementById('ssid').value.trim();
  var p=document.getElementById('pass').value;
  if(!s){document.getElementById('st').textContent='Inserisci il nome rete';return;}
  document.getElementById('st').textContent='Salvataggio…';
  var r=await fetch('/api/wifi',{method:'POST',headers:{'Content-Type':'application/json'},body:JSON.stringify({ssid:s,pass:p})});
  var d=await r.json();
  document.getElementById('st').textContent=d.ok?'Salvato! Il dispositivo si riavvierà…':'Errore: '+d.error;
}
</script>
</body>
</html>
)rawliteral";
