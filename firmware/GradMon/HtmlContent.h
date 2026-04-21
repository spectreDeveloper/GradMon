#pragma once

const char HTML_INDEX[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="it">
<head>
<meta charset="UTF-8">
<meta name="viewport" content="width=device-width,initial-scale=1">
<title>GradMon</title>
<style>
*{box-sizing:border-box;margin:0;padding:0}
body{background:#0d0d0d;color:#e8e8e8;font-family:-apple-system,BlinkMacSystemFont,'Segoe UI',sans-serif;max-width:500px;margin:0 auto;padding:16px 12px 40px}

/* Header */
.hdr{text-align:center;padding:20px 0 16px}
.hdr h1{font-size:28px;letter-spacing:4px;color:#f5c518;font-weight:800}
.hdr p{font-size:11px;color:#555;letter-spacing:2px;margin-top:4px;text-transform:uppercase}

/* Cards */
.card{background:#161616;border:1px solid #252525;border-radius:14px;padding:18px;margin-bottom:14px}
.card-title{font-size:10px;color:#666;text-transform:uppercase;letter-spacing:2px;margin-bottom:14px;font-weight:600}

/* Inputs */
input,select{width:100%;background:#1e1e1e;border:1px solid #333;color:#e8e8e8;border-radius:8px;padding:11px 14px;font-size:14px;margin-bottom:10px;outline:none;transition:.2s border-color}
input:focus,select:focus{border-color:#f5c518}
input[type=password]{letter-spacing:2px}

/* Buttons */
.btn{display:block;width:100%;border:none;border-radius:8px;padding:13px;font-size:14px;font-weight:700;cursor:pointer;transition:.15s}
.btn-gold{background:#f5c518;color:#111}
.btn-gold:hover{background:#e0b010}
.btn-dim{background:#222;color:#bbb;margin-top:8px}
.btn-dim:hover{background:#2e2e2e}
.btn-green{background:#27ae60;color:#fff;margin-top:8px}
.btn-green:hover{background:#219a52}

/* Status messages */
.status{font-size:12px;text-align:center;margin-top:8px;min-height:16px;color:#666}
.status.ok{color:#27ae60}
.status.err{color:#e74c3c}
.status.info{color:#f5c518}

/* Search results */
.results{margin-top:12px}
.result{display:flex;align-items:center;gap:12px;background:#1e1e1e;border:1px solid #2a2a2a;border-radius:10px;padding:10px;margin-bottom:8px;cursor:pointer;transition:.15s}
.result:hover{border-color:#444}
.result.sel{border-color:#f5c518;background:#1e1c00}
.result img{width:44px;height:62px;object-fit:contain;border-radius:5px;background:#111;flex-shrink:0}
.result-info .name{font-size:13px;font-weight:600;line-height:1.3;color:#eee}
.result-info .set{font-size:11px;color:#777;margin-top:3px}
.result-info .num{font-size:10px;color:#555;margin-top:1px}

/* Grade selector */
.row2{display:grid;grid-template-columns:1fr 1fr;gap:10px}
.row2 select{margin-bottom:0}

/* Price display */
.price-box{background:#111;border:1px solid #2a2a2a;border-radius:10px;padding:16px;text-align:center;margin:12px 0}
.price-val{font-size:32px;font-weight:800;color:#f5c518;letter-spacing:-1px}
.price-label{font-size:10px;color:#555;text-transform:uppercase;letter-spacing:2px;margin-top:4px}

/* ── OLED preview (128×64 fedele) ─────────────────────────── */
.oled-wrap{background:#000;border:3px solid #1a1a1a;border-radius:6px;overflow:hidden;width:256px;height:128px;margin:0 auto;image-rendering:pixelated;position:relative;box-shadow:0 0 20px #0004}
.oled-inner{position:absolute;inset:0;padding:0;display:flex;flex-direction:column;justify-content:flex-start}
.oled-screen{width:100%;height:100%;background:#000;display:flex;flex-direction:column;padding:4px 5px 3px}
/* Scale ×2 visually */
.oled-wrap{transform-origin:top center}
.oled-line{font-family:'Courier New',Courier,monospace;white-space:nowrap;overflow:hidden;line-height:1}
.oled-sm{font-size:13px;color:#90ee90;margin-bottom:1px}   /* Plain_10 ≈ */
.oled-lg{font-size:20px;color:#90ee90;font-weight:bold;line-height:1.1} /* Plain_16 ≈ */
.oled-hr{border:none;border-top:1px solid #1a4a1a;margin:3px 0 2px}
.oled-row{display:flex;justify-content:space-between;align-items:baseline}
.oled-live{font-size:11px;color:#3a8a3a;text-align:center;margin-top:3px;letter-spacing:1px}
.oled-idle{font-size:13px;color:#2a5a2a;text-align:center;margin-top:30px}

.hidden{display:none}
</style>
</head>
<body>
<div class="hdr">
  <h1>GRADMON</h1>
  <p>Smart Card Display</p>
</div>

<!-- API Config -->
<div class="card">
  <div class="card-title">Configurazione API</div>
  <input type="password" id="apiKey" placeholder="RapidAPI Key (TCGgo)" autocomplete="new-password">
  <select id="currency">
    <option value="USD">USD — Dollaro ($)</option>
    <option value="EUR">EUR — Euro (€)</option>
    <option value="GBP">GBP — Sterlina (£)</option>
    <option value="JPY">JPY — Yen (¥)</option>
    <option value="CAD">CAD — Dollaro canadese</option>
  </select>
  <button class="btn btn-gold" onclick="saveConfig()">Salva configurazione</button>
  <div class="status" id="cfgStatus"></div>
</div>

<!-- Cerca carta -->
<div class="card">
  <div class="card-title">Cerca carta</div>
  <input type="text" id="searchQ" placeholder="es. Charizard, Pikachu V, Blastoise..." onkeydown="if(event.key==='Enter')searchCards()">
  <button class="btn btn-dim" onclick="searchCards()">Cerca</button>
  <div class="results" id="results"></div>
</div>

<!-- Selezione grado (nascosta finché non si sceglie una carta) -->
<div class="card hidden" id="gradeCard">
  <div class="card-title" id="selectedCardTitle">Seleziona grado</div>
  <div class="row2">
    <select id="grader" onchange="onGradeChange()">
      <option value="psa">PSA</option>
      <option value="bgs">BGS / Beckett</option>
      <option value="cgc">CGC</option>
      <option value="sgc">SGC</option>
    </select>
    <select id="grade" onchange="onGradeChange()">
      <option value="10">10</option>
      <option value="9_5">9.5</option>
      <option value="9">9</option>
      <option value="8_5">8.5</option>
      <option value="8">8</option>
      <option value="7">7</option>
      <option value="6">6</option>
      <option value="5">5</option>
      <option value="4">4</option>
      <option value="3">3</option>
      <option value="2">2</option>
      <option value="1">1</option>
    </select>
  </div>

  <div class="price-box" id="priceBox">
    <div class="price-val" id="priceVal">—</div>
    <div class="price-label" id="priceLabel">seleziona un grado</div>
  </div>

  <button class="btn btn-green hidden" id="sendBtn" onclick="sendToDisplay()">
    ▶ Invia al display OLED
  </button>
  <div class="status" id="gradeStatus"></div>
</div>

<!-- Preview OLED -->
<div class="card">
  <div class="card-title">Preview display OLED (128×64)</div>
  <div class="oled-wrap">
    <div class="oled-screen" id="oledScreen">
      <div class="oled-idle">[ nessuna carta ]</div>
    </div>
  </div>
</div>

<script>
var SEL = null;          // carta selezionata (oggetto completo con prices)
var LAST_PRICE = null;   // {val, formatted}
var CURRENCY = 'USD';

// ── Utilità ─────────────────────────────────────────────────────────────────
var SYM = {USD:'$',EUR:'€',GBP:'£',JPY:'¥',CAD:'CA$'};
function sym(c){return SYM[c]||c+' ';}

function fmt(v,c){
  if(v==null||v===undefined||isNaN(v)) return null;
  v = parseFloat(v);
  var s = sym(c);
  // No decimali se cifra tonda, altrimenti 2
  var decimals = (v%1===0)?0:2;
  return s + v.toLocaleString('it-IT',{minimumFractionDigits:decimals,maximumFractionDigits:decimals});
}

function trunc(s,n){if(!s)return'';return s.length<=n?s:s.slice(0,n-1)+'~';}

function setStatus(id,msg,cls){
  var el=document.getElementById(id);
  el.textContent=msg; el.className='status '+(cls||'');
}

// ── Salva config ──────────────────────────────────────────────────────────────
async function saveConfig(){
  var k=document.getElementById('apiKey').value.trim();
  CURRENCY=document.getElementById('currency').value;
  if(!k){setStatus('cfgStatus','Inserisci la API key','err');return;}
  setStatus('cfgStatus','Salvataggio...','info');
  var r=await fetch('/api/config',{method:'POST',headers:{'Content-Type':'application/json'},
    body:JSON.stringify({apiKey:k,currency:CURRENCY})});
  var d=await r.json();
  document.getElementById('apiKey').value='';
  setStatus('cfgStatus',d.ok?'Salvato!':'Errore: '+d.error,d.ok?'ok':'err');
}

// ── Ricerca ────────────────────────────────────────────────────────────────────
async function searchCards(){
  var q=document.getElementById('searchQ').value.trim();
  if(!q)return;
  document.getElementById('results').innerHTML='<div class="status info">Ricerca in corso...</div>';
  SEL=null;
  document.getElementById('gradeCard').classList.add('hidden');

  var r=await fetch('/api/search?q='+encodeURIComponent(q));
  var d=await r.json();
  if(!d.ok){document.getElementById('results').innerHTML='<div class="status err">'+d.error+'</div>';return;}
  var cards=d.results||[];
  if(!cards.length){document.getElementById('results').innerHTML='<div class="status">Nessun risultato trovato</div>';return;}

  var html='';
  cards.forEach(function(c,i){
    var setInfo=(c.setName||'')+(c.cardNumber?' &nbsp;#'+c.cardNumber:'');
    html+='<div class="result" id="ri'+i+'" onclick="pick('+i+')">';
    if(c.imageUrl)
      html+='<img src="'+c.imageUrl+'" loading="lazy" onerror="this.style.visibility=\'hidden\'">';
    else
      html+='<div style="width:44px;height:62px;background:#111;border-radius:5px;flex-shrink:0"></div>';
    html+='<div class="result-info"><div class="name">'+c.name+'</div><div class="set">'+setInfo+'</div></div></div>';
  });
  document.getElementById('results').innerHTML=html;
  // Salviamo i dati delle carte nell'array globale
  window._cards=cards;
}

// ── Selezione carta ───────────────────────────────────────────────────────────
function pick(i){
  document.querySelectorAll('.result').forEach(function(e){e.classList.remove('sel');});
  document.getElementById('ri'+i).classList.add('sel');
  SEL=window._cards[i];
  LAST_PRICE=null;
  document.getElementById('gradeCard').classList.remove('hidden');
  document.getElementById('selectedCardTitle').textContent=SEL.name;
  document.getElementById('sendBtn').classList.add('hidden');
  setStatus('gradeStatus','','');
  onGradeChange();
}

// ── Cambio grader/grade: estrae prezzo dai dati già ricevuti ──────────────────
function onGradeChange(){
  if(!SEL) return;
  var grader = document.getElementById('grader').value;   // 'psa','bgs','cgc','sgc'
  var grade  = document.getElementById('grade').value;    // '10','9_5','9',...
  CURRENCY   = document.getElementById('currency').value;

  var priceKey = grader + grade;   // es. 'psa10', 'bgs9_5'
  var val = SEL.prices&&SEL.prices.graded&&SEL.prices.graded[grader]
              ? SEL.prices.graded[grader][priceKey] : undefined;

  // Fallback su prezzi market (ungraded)
  if(val==null||val===undefined){
    if(CURRENCY==='EUR') val = SEL.prices&&SEL.prices.eur;
    else                 val = SEL.prices&&SEL.prices.usd;
  }

  var formatted = fmt(val, CURRENCY);
  var label = document.getElementById('grader').options[document.getElementById('grader').selectedIndex].text;

  if(formatted){
    document.getElementById('priceVal').textContent   = formatted;
    document.getElementById('priceLabel').textContent = label+' '+grade.replace('_','.')+' — prezzo corrente';
    LAST_PRICE = {val:val, formatted:formatted};
    document.getElementById('sendBtn').classList.remove('hidden');
  } else {
    document.getElementById('priceVal').textContent   = 'N/D';
    document.getElementById('priceLabel').textContent = 'Prezzo non disponibile per questo grado';
    LAST_PRICE = null;
    document.getElementById('sendBtn').classList.add('hidden');
  }

  updateOledPreview();
}

// ── Invia al display ──────────────────────────────────────────────────────────
async function sendToDisplay(){
  if(!SEL||!LAST_PRICE) return;
  var graderVal = document.getElementById('grader').value;          // 'psa','bgs'…
  var gradeVal  = document.getElementById('grade').value;           // '10','9_5'…
  var graderKey = graderVal + gradeVal;                             // 'psa10','bgs9_5'
  var gradeDisp = graderVal.toUpperCase()+' '+gradeVal.replace('_','.');

  var payload={
    name:      SEL.name,
    setName:   SEL.setName||'',
    cardNumber:SEL.cardNumber||'',
    grade:     gradeDisp,
    price:     LAST_PRICE.formatted,
    cardId:    SEL.id||'',          // per il refresh 24h
    graderKey: graderKey            // es. "psa10", "bgs9_5"
  };
  setStatus('gradeStatus','Invio in corso...','info');
  var r=await fetch('/api/display',{method:'POST',headers:{'Content-Type':'application/json'},body:JSON.stringify(payload)});
  var d=await r.json();
  setStatus('gradeStatus',d.ok?'Inviato al display!':'Errore: '+d.error,d.ok?'ok':'err');
}

// ── Preview OLED fedele ───────────────────────────────────────────────────────
function updateOledPreview(){
  var scr = document.getElementById('oledScreen');
  if(!SEL){
    scr.innerHTML='<div class="oled-idle">[ nessuna carta ]</div>';
    return;
  }

  var grader = document.getElementById('grader').value.toUpperCase();
  var grade  = document.getElementById('grade').value.replace('_','.');
  var name   = trunc(SEL.name||'',21);
  var setLine= trunc((SEL.setName||'')+(SEL.cardNumber?' #'+SEL.cardNumber:''),21);
  var gradeStr = trunc(grader+' '+grade, 8);
  var priceStr = LAST_PRICE ? trunc(LAST_PRICE.formatted, 9) : 'N/D';

  // Layout fedele al display fisico:
  // Riga 1: nome (Plain_10)
  // Riga 2: set #num (Plain_10)
  // ══ separatore doppio
  // Riga 3: [grade sx] [price dx] (Plain_16)
  // Riga 4: * LIVE PRICE * (Plain_10, centrato)
  scr.innerHTML=
    '<div class="oled-line oled-sm">'+esc(name)+'</div>'+
    '<div class="oled-line oled-sm">'+esc(setLine)+'</div>'+
    '<hr class="oled-hr">'+
    '<div class="oled-row">'+
      '<div class="oled-line oled-lg">'+esc(gradeStr)+'</div>'+
      '<div class="oled-line oled-lg">'+esc(priceStr)+'</div>'+
    '</div>'+
    '<div class="oled-live">* LIVE PRICE *</div>';
}

function esc(s){return s.replace(/&/g,'&amp;').replace(/</g,'&lt;').replace(/>/g,'&gt;');}

// ── Init: carica currency salvata ─────────────────────────────────────────────
fetch('/api/status').then(function(r){return r.json();}).then(function(d){
  if(d.currency){
    CURRENCY=d.currency;
    document.getElementById('currency').value=d.currency;
  }
  if(d.cardName){
    var scr=document.getElementById('oledScreen');
    scr.innerHTML=
      '<div class="oled-line oled-sm">'+esc(trunc(d.cardName,21))+'</div>'+
      '<div class="oled-line oled-sm">'+esc(trunc(d.cardSet||'',21))+'</div>'+
      '<hr class="oled-hr">'+
      '<div class="oled-row">'+
        '<div class="oled-line oled-lg">'+esc(trunc(d.cardGrade||'',8))+'</div>'+
        '<div class="oled-line oled-lg">'+esc(trunc(d.cardPrice||'',9))+'</div>'+
      '</div>'+
      '<div class="oled-live">* LIVE PRICE *</div>';
  }
}).catch(function(){});
</script>
</body>
</html>
)rawliteral";

// ── WiFi setup page (AP mode) ────────────────────────────────────────────────
const char HTML_WIFI_SETUP[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="it">
<head>
<meta charset="UTF-8">
<meta name="viewport" content="width=device-width,initial-scale=1">
<title>GradMon — Setup WiFi</title>
<style>
*{box-sizing:border-box;margin:0;padding:0}
body{background:#0d0d0d;color:#e8e8e8;font-family:-apple-system,sans-serif;max-width:360px;margin:0 auto;padding:40px 16px}
h1{text-align:center;font-size:26px;color:#f5c518;letter-spacing:4px;font-weight:800;margin-bottom:6px}
p{text-align:center;color:#555;font-size:12px;letter-spacing:2px;text-transform:uppercase;margin-bottom:28px}
.card{background:#161616;border:1px solid #252525;border-radius:14px;padding:22px}
input{width:100%;background:#1e1e1e;border:1px solid #333;color:#e8e8e8;border-radius:8px;padding:12px 14px;font-size:14px;margin-bottom:12px;outline:none}
input:focus{border-color:#f5c518}
button{width:100%;background:#f5c518;color:#111;border:none;border-radius:8px;padding:13px;font-size:14px;font-weight:700;cursor:pointer}
button:hover{background:#e0b010}
.status{font-size:12px;color:#666;text-align:center;margin-top:12px;min-height:16px}
</style>
</head>
<body>
<h1>GRADMON</h1>
<p>Configurazione WiFi</p>
<div class="card">
  <input type="text" id="ssid" placeholder="Nome rete WiFi (SSID)" autocomplete="off">
  <input type="password" id="pass" placeholder="Password WiFi">
  <button onclick="save()">Connetti e riavvia</button>
  <div class="status" id="st"></div>
</div>
<script>
async function save(){
  var s=document.getElementById('ssid').value.trim();
  var p=document.getElementById('pass').value;
  if(!s){document.getElementById('st').textContent='Inserisci il nome rete';return;}
  document.getElementById('st').textContent='Salvataggio...';
  var r=await fetch('/api/wifi',{method:'POST',headers:{'Content-Type':'application/json'},body:JSON.stringify({ssid:s,pass:p})});
  var d=await r.json();
  document.getElementById('st').textContent=d.ok?'Salvato! Riavvio in corso...':'Errore: '+d.error;
}
</script>
</body>
</html>
)rawliteral";
