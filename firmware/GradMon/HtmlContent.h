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
body{background:#0d0d0d;color:#e8e8e8;font-family:-apple-system,BlinkMacSystemFont,'Segoe UI',sans-serif;max-width:520px;margin:0 auto;padding:16px 12px 48px}

.hdr{text-align:center;padding:22px 0 18px}
.hdr h1{font-size:30px;letter-spacing:5px;color:#f5c518;font-weight:900}
.hdr p{font-size:10px;color:#444;letter-spacing:3px;margin-top:5px;text-transform:uppercase}

.card{background:#161616;border:1px solid #222;border-radius:14px;padding:18px;margin-bottom:14px}
.card-title{font-size:10px;color:#555;text-transform:uppercase;letter-spacing:2px;margin-bottom:14px;font-weight:600}

input,select{width:100%;background:#1e1e1e;border:1px solid #2e2e2e;color:#e8e8e8;border-radius:8px;padding:11px 14px;font-size:14px;margin-bottom:10px;outline:none;transition:.15s border-color}
input:focus,select:focus{border-color:#f5c518}

.btn{display:block;width:100%;border:none;border-radius:8px;padding:13px;font-size:14px;font-weight:700;cursor:pointer;transition:.15s;text-align:center}
.btn-gold{background:#f5c518;color:#111}.btn-gold:hover{background:#e0b010}
.btn-dim{background:#1e1e1e;color:#aaa;border:1px solid #2e2e2e}.btn-dim:hover{background:#252525}
.btn-green{background:#1a6b3a;color:#fff;margin-top:10px}.btn-green:hover{background:#1e7d45}
.btn-green.active{background:#27ae60}

.status{font-size:12px;text-align:center;margin-top:8px;min-height:16px;color:#555}
.status.ok{color:#27ae60}.status.err{color:#e74c3c}.status.info{color:#f5c518}

/* Risultati ricerca */
.result{display:flex;align-items:center;gap:12px;background:#1a1a1a;border:1px solid #222;border-radius:10px;padding:10px 12px;margin-top:8px;cursor:pointer;transition:.15s}
.result:hover{border-color:#3a3a3a}
.result.sel{border-color:#f5c518;background:#141200}
.result img,.result .img-ph{width:46px;height:64px;object-fit:contain;border-radius:5px;background:#111;flex-shrink:0}
.result .img-ph{display:flex;align-items:center;justify-content:center;color:#333;font-size:20px}
.r-name{font-size:13px;font-weight:600;color:#eee;line-height:1.3}
.r-sub{font-size:11px;color:#666;margin-top:2px}
.r-badge{display:inline-block;background:#222;border-radius:4px;padding:2px 6px;font-size:10px;color:#888;margin-top:4px;margin-right:3px}

/* Card espansa (dettaglio) */
.card-detail{display:flex;gap:14px;margin-bottom:16px}
.card-detail img{width:90px;height:126px;object-fit:contain;border-radius:8px;background:#111;flex-shrink:0;box-shadow:0 4px 20px #0008}
.card-detail .info{flex:1;min-width:0}
.detail-name{font-size:17px;font-weight:700;color:#fff;line-height:1.2;margin-bottom:6px}
.detail-sub{font-size:12px;color:#777;margin-bottom:4px}
.detail-badges{display:flex;flex-wrap:wrap;gap:4px;margin-top:6px}
.badge{background:#1e1e1e;border:1px solid #2a2a2a;border-radius:5px;padding:3px 8px;font-size:10px;color:#888}
.badge.rare{color:#f5c518;border-color:#3a3200}
.badge.hp{color:#e74c3c;border-color:#3a1a1a}

/* Griglia opzioni prezzo */
.price-grid-title{font-size:10px;color:#555;text-transform:uppercase;letter-spacing:2px;margin-bottom:10px;font-weight:600}
.price-grid{display:grid;grid-template-columns:repeat(2,1fr);gap:8px;margin-bottom:10px}
.price-opt{background:#1a1a1a;border:1px solid #222;border-radius:10px;padding:10px 12px;cursor:pointer;transition:.15s;position:relative}
.price-opt:hover{border-color:#3a3a3a}
.price-opt.sel{border-color:#f5c518;background:#141200}
.price-opt.sel::after{content:'✓';position:absolute;top:7px;right:9px;color:#f5c518;font-size:11px;font-weight:700}
.p-group{font-size:9px;color:#555;text-transform:uppercase;letter-spacing:1.5px;margin-bottom:3px}
.p-label{font-size:12px;color:#bbb;font-weight:600;margin-bottom:5px}
.p-val{font-size:20px;color:#f5c518;font-weight:800;letter-spacing:-0.5px}
.p-cur{font-size:10px;color:#666;margin-left:2px}
.no-prices{color:#555;font-size:13px;text-align:center;padding:20px;border:1px dashed #222;border-radius:10px}

/* Preview OLED (128×64 pixel × 2 = 256×128) */
.oled-outer{background:#050505;border-radius:8px;padding:12px;margin-top:4px}
.oled-label{font-size:10px;color:#333;text-transform:uppercase;letter-spacing:2px;margin-bottom:8px;text-align:center}
.oled-frame{width:256px;height:128px;background:#000;border:2px solid #111;border-radius:4px;margin:0 auto;overflow:hidden;display:flex;flex-direction:column;padding:4px 6px 3px;box-shadow:inset 0 0 10px #001a00}
.ol{font-family:'Courier New',monospace;overflow:hidden;white-space:nowrap;color:#5aff5a;text-shadow:0 0 4px #00ff0044}
.ol-sm{font-size:12px;line-height:1.45}
.ol-lg{font-size:18px;font-weight:700;line-height:1.2}
.ol-hr{border:none;border-top:1px solid #0a2a0a;margin:3px 0}
.ol-dhr{border:none;border-top:2px solid #0a2a0a;margin:2px 0;box-shadow:0 1px 0 #0a2a0a}
.ol-row{display:flex;justify-content:space-between;align-items:baseline}
.ol-live{font-size:10px;color:#2a6a2a;text-align:center;margin-top:4px;letter-spacing:1px;font-family:monospace}
.ol-idle{font-size:12px;color:#1a3a1a;text-align:center;padding-top:44px;font-family:monospace}
.ol-boot{font-size:11px;color:#2a5a2a;text-align:center;padding-top:38px;font-family:monospace;letter-spacing:1px}

.hidden{display:none}
</style>
</head>
<body>

<div class="hdr"><h1>GRADMON</h1><p>Smart Graded Card Display</p></div>

<!-- Config -->
<div class="card">
  <div class="card-title">Configurazione API</div>
  <input type="password" id="apiKey" placeholder="RapidAPI Key" autocomplete="new-password">
  <select id="currency">
    <option value="EUR">EUR — Euro (€)</option>
    <option value="USD">USD — Dollaro ($)</option>
    <option value="GBP">GBP — Sterlina (£)</option>
    <option value="JPY">JPY — Yen (¥)</option>
  </select>
  <button class="btn btn-gold" onclick="saveConfig()">Salva configurazione</button>
  <div class="status" id="cfgSt"></div>
</div>

<!-- Ricerca -->
<div class="card">
  <div class="card-title">Cerca carta</div>
  <input type="text" id="searchQ" placeholder="es. Charizard, Pikachu V, Blastoise EX…" onkeydown="if(event.key==='Enter')doSearch()">
  <button class="btn btn-dim" onclick="doSearch()">Cerca</button>
  <div id="results"></div>
</div>

<!-- Dettaglio carta + prezzi -->
<div class="card hidden" id="detailCard">
  <div class="card-title" id="detailTitle">Seleziona prezzo</div>
  <div class="card-detail">
    <img id="cardImg" src="" alt="" onerror="this.style.display='none'">
    <div class="info">
      <div class="detail-name" id="dName"></div>
      <div class="detail-sub" id="dSet"></div>
      <div class="detail-sub" id="dNum"></div>
      <div class="detail-badges" id="dBadges"></div>
    </div>
  </div>
  <div class="price-grid-title">Opzioni prezzo disponibili</div>
  <div class="price-grid" id="priceGrid"></div>
  <button class="btn btn-green hidden" id="sendBtn" onclick="sendToDisplay()">
    ▶ Invia al display OLED
  </button>
  <div class="status" id="sendSt"></div>
</div>

<!-- Preview OLED -->
<div class="card">
  <div class="card-title">Preview display OLED</div>
  <div class="oled-outer">
    <div class="oled-label">128 × 64 px</div>
    <div class="oled-frame" id="oledFrame">
      <div class="ol-boot">&#9673; GRADMON v1.0<br><br>Pronto.</div>
    </div>
  </div>
</div>

<script>
var CARDS = [], SEL = null, SEL_PRICE = null;
var CURRENCY = 'EUR';
var SYM = {EUR:'€',USD:'$',GBP:'£',JPY:'¥'};

function sym(c){return SYM[c]||c;}
function fmt(v,c){
  if(!v||isNaN(v))return null;
  v=parseFloat(v);
  return sym(c)+(v%1===0?v.toLocaleString('it-IT'):v.toLocaleString('it-IT',{minimumFractionDigits:2,maximumFractionDigits:2}));
}
function trunc(s,n){if(!s)return'';s=String(s);return s.length<=n?s:s.slice(0,n-1)+'~';}
function esc(s){return String(s||'').replace(/&/g,'&amp;').replace(/</g,'&lt;').replace(/>/g,'&gt;');}
function setSt(id,msg,cls){var e=document.getElementById(id);e.textContent=msg;e.className='status '+(cls||'');}

// ── Config ────────────────────────────────────────────────────────────────────
async function saveConfig(){
  var k=document.getElementById('apiKey').value.trim();
  CURRENCY=document.getElementById('currency').value;
  if(!k){setSt('cfgSt','Inserisci la API key','err');return;}
  setSt('cfgSt','Salvataggio…','info');
  var r=await fetch('/api/config',{method:'POST',headers:{'Content-Type':'application/json'},body:JSON.stringify({apiKey:k,currency:CURRENCY})});
  var d=await r.json();
  document.getElementById('apiKey').value='';
  setSt('cfgSt',d.ok?'Salvato!':'Errore: '+d.error,d.ok?'ok':'err');
}

// ── Ricerca ───────────────────────────────────────────────────────────────────
async function doSearch(){
  var q=document.getElementById('searchQ').value.trim();
  if(!q)return;
  document.getElementById('results').innerHTML='<div class="status info" style="margin-top:12px">Ricerca in corso…</div>';
  document.getElementById('detailCard').classList.add('hidden');
  SEL=null; SEL_PRICE=null;

  var r=await fetch('/api/search?q='+encodeURIComponent(q));
  var d=await r.json();
  if(!d.ok){document.getElementById('results').innerHTML='<div class="status err" style="margin-top:12px">'+d.error+'</div>';return;}
  CARDS=d.results||[];
  if(!CARDS.length){document.getElementById('results').innerHTML='<div class="status" style="margin-top:12px">Nessuna carta trovata</div>';return;}

  var html='';
  CARDS.forEach(function(c,i){
    var badge=(c.rarity?'<span class="r-badge">'+esc(c.rarity)+'</span>':'')
             +(c.hp?'<span class="r-badge">HP '+c.hp+'</span>':'');
    html+='<div class="result" id="ri'+i+'" onclick="pick('+i+')">';
    if(c.imageUrl)
      html+='<img src="'+esc(c.imageUrl)+'" loading="lazy" onerror="this.className=\'img-ph\';this.outerHTML=\'<div class=\\\"img-ph\\\">&#129655;</div>\'">';
    else
      html+='<div class="img-ph">&#129655;</div>';
    html+='<div><div class="r-name">'+esc(c.nameNum||c.name)+'</div>'
        +'<div class="r-sub">'+esc(c.setName||'—')+(c.cardNumber?' &nbsp;#'+c.cardNumber:'')+'</div>'
        +'<div>'+badge+'</div></div></div>';
  });
  document.getElementById('results').innerHTML=html;
}

// ── Selezione carta ───────────────────────────────────────────────────────────
function pick(i){
  document.querySelectorAll('.result').forEach(function(e){e.classList.remove('sel');});
  document.getElementById('ri'+i).classList.add('sel');
  SEL=CARDS[i]; SEL_PRICE=null;

  // Popola dettaglio
  document.getElementById('detailTitle').textContent=SEL.name;
  document.getElementById('dName').textContent=SEL.nameNum||SEL.name;
  document.getElementById('dSet').textContent=SEL.setName||'Set sconosciuto';
  document.getElementById('dNum').textContent=SEL.cardNumber?'#'+SEL.cardNumber+' — '+SEL.supertype:'';

  var img=document.getElementById('cardImg');
  if(SEL.imageUrl){img.src=SEL.imageUrl;img.style.display='';}
  else img.style.display='none';

  var badges='';
  if(SEL.rarity) badges+='<span class="badge rare">'+esc(SEL.rarity)+'</span>';
  if(SEL.hp)     badges+='<span class="badge hp">HP '+SEL.hp+'</span>';
  if(SEL.supertype) badges+='<span class="badge">'+esc(SEL.supertype)+'</span>';
  document.getElementById('dBadges').innerHTML=badges;

  // Griglia prezzi
  buildPriceGrid(SEL.priceOptions||[]);

  document.getElementById('detailCard').classList.remove('hidden');
  document.getElementById('sendBtn').classList.add('hidden');
  setSt('sendSt','','');
  updateOled();
  document.getElementById('detailCard').scrollIntoView({behavior:'smooth',block:'nearest'});
}

// ── Griglia opzioni prezzo ────────────────────────────────────────────────────
function buildPriceGrid(opts){
  if(!opts.length){
    document.getElementById('priceGrid').innerHTML='<div class="no-prices" style="grid-column:1/-1">Nessun prezzo disponibile per questa carta</div>';
    return;
  }
  var html='';
  opts.forEach(function(o,i){
    var val=fmt(o.value, o.currency||CURRENCY)||'—';
    html+='<div class="price-opt" id="po'+i+'" onclick="selPrice('+i+')">'
        +'<div class="p-group">'+esc(o.group)+'</div>'
        +'<div class="p-label">'+esc(o.label)+'</div>'
        +'<div class="p-val">'+esc(val)+'</div>'
        +'</div>';
  });
  document.getElementById('priceGrid').innerHTML=html;
}

function selPrice(i){
  document.querySelectorAll('.price-opt').forEach(function(e){e.classList.remove('sel');});
  document.getElementById('po'+i).classList.add('sel');
  var opt=SEL.priceOptions[i];
  SEL_PRICE={
    key:      opt.key,
    label:    opt.label,
    group:    opt.group,
    value:    opt.value,
    currency: opt.currency||CURRENCY,
    formatted:fmt(opt.value, opt.currency||CURRENCY)
  };
  document.getElementById('sendBtn').classList.remove('hidden');
  setSt('sendSt','','');
  updateOled();
}

// ── Invia al display ──────────────────────────────────────────────────────────
async function sendToDisplay(){
  if(!SEL||!SEL_PRICE) return;

  // Etichetta grado: "PSA 10" o "CM NM" o "Media 30gg" ecc.
  var gradeLabel = (SEL_PRICE.group && SEL_PRICE.group!=='Cardmarket'&&SEL_PRICE.group!=='TCGPlayer')
    ? SEL_PRICE.group+' '+SEL_PRICE.label   // es. "PSA 10"
    : SEL_PRICE.label;                       // es. "Near Mint"

  var payload={
    name:      SEL.name,
    setName:   SEL.setName||'',
    cardNumber:SEL.cardNumber||'',
    grade:     gradeLabel,
    price:     SEL_PRICE.formatted,
    cardId:    SEL.id,
    priceKey:  SEL_PRICE.key   // per il refresh 24h
  };

  setSt('sendSt','Invio…','info');
  var r=await fetch('/api/display',{method:'POST',headers:{'Content-Type':'application/json'},body:JSON.stringify(payload)});
  var d=await r.json();
  if(d.ok){
    setSt('sendSt','✓ Inviato al display!','ok');
    document.getElementById('sendBtn').classList.add('active');
    setTimeout(function(){document.getElementById('sendBtn').classList.remove('active');},2000);
  } else {
    setSt('sendSt','Errore: '+d.error,'err');
  }
}

// ── Preview OLED ─────────────────────────────────────────────────────────────
function updateOled(){
  var f=document.getElementById('oledFrame');
  if(!SEL){f.innerHTML='<div class="ol-boot">&#9673; GRADMON v1.0<br><br>Pronto.</div>';return;}

  var name   =trunc(SEL.nameNum||SEL.name, 21);
  var setLine=trunc((SEL.setName||'')+(SEL.cardNumber?' #'+SEL.cardNumber:''),21);
  var grade  ='', price='';

  if(SEL_PRICE){
    var g=(SEL_PRICE.group&&SEL_PRICE.group!=='Cardmarket'&&SEL_PRICE.group!=='TCGPlayer')
          ? SEL_PRICE.group+' '+SEL_PRICE.label : SEL_PRICE.label;
    grade=trunc(g,10);
    price=trunc(SEL_PRICE.formatted,9);
  } else {
    grade='— seleziona'; price='prezzo';
  }

  f.innerHTML=
    '<div class="ol ol-sm">'+esc(name)+'</div>'+
    '<div class="ol ol-sm">'+esc(setLine)+'</div>'+
    '<div class="ol-dhr"></div>'+
    '<div class="ol-row">'+
      '<div class="ol ol-lg">'+esc(grade)+'</div>'+
      '<div class="ol ol-lg">'+esc(price)+'</div>'+
    '</div>'+
    '<div class="ol-live">&#9679; LIVE PRICE &#9679;</div>';
}

// ── Init ──────────────────────────────────────────────────────────────────────
fetch('/api/status').then(function(r){return r.json();}).then(function(d){
  if(d.currency){CURRENCY=d.currency;document.getElementById('currency').value=d.currency;}
  if(d.cardName){
    var n=trunc(d.cardName,21), s=trunc(d.cardSet||'',21);
    var g=trunc(d.cardGrade||'',10), p=trunc(d.cardPrice||'',9);
    document.getElementById('oledFrame').innerHTML=
      '<div class="ol ol-sm">'+esc(n)+'</div>'+
      '<div class="ol ol-sm">'+esc(s)+'</div>'+
      '<div class="ol-dhr"></div>'+
      '<div class="ol-row"><div class="ol ol-lg">'+esc(g)+'</div><div class="ol ol-lg">'+esc(p)+'</div></div>'+
      '<div class="ol-live">&#9679; LIVE PRICE &#9679;</div>';
  }
}).catch(function(){});
</script>
</body>
</html>
)rawliteral";

// ── WiFi setup (AP mode) ─────────────────────────────────────────────────────
const char HTML_WIFI_SETUP[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="it">
<head>
<meta charset="UTF-8">
<meta name="viewport" content="width=device-width,initial-scale=1">
<title>GradMon — Setup</title>
<style>
*{box-sizing:border-box;margin:0;padding:0}
body{background:#0d0d0d;color:#e8e8e8;font-family:-apple-system,sans-serif;max-width:360px;margin:0 auto;padding:48px 16px}
h1{text-align:center;font-size:26px;color:#f5c518;letter-spacing:5px;font-weight:900;margin-bottom:4px}
p{text-align:center;color:#444;font-size:11px;letter-spacing:2px;text-transform:uppercase;margin-bottom:28px}
.card{background:#161616;border:1px solid #222;border-radius:14px;padding:22px}
input{width:100%;background:#1e1e1e;border:1px solid #2e2e2e;color:#e8e8e8;border-radius:8px;padding:12px 14px;font-size:14px;margin-bottom:12px;outline:none}
input:focus{border-color:#f5c518}
button{width:100%;background:#f5c518;color:#111;border:none;border-radius:8px;padding:13px;font-size:14px;font-weight:700;cursor:pointer}
button:hover{background:#e0b010}
.st{font-size:12px;color:#555;text-align:center;margin-top:12px;min-height:16px}
</style>
</head>
<body>
<h1>GRADMON</h1><p>Configurazione WiFi</p>
<div class="card">
  <input type="text" id="ssid" placeholder="Nome rete WiFi (SSID)" autocomplete="off">
  <input type="password" id="pass" placeholder="Password WiFi">
  <button onclick="save()">Connetti e riavvia</button>
  <div class="st" id="st"></div>
</div>
<script>
async function save(){
  var s=document.getElementById('ssid').value.trim();
  var p=document.getElementById('pass').value;
  if(!s){document.getElementById('st').textContent='Inserisci il nome rete';return;}
  document.getElementById('st').textContent='Salvataggio…';
  var r=await fetch('/api/wifi',{method:'POST',headers:{'Content-Type':'application/json'},body:JSON.stringify({ssid:s,pass:p})});
  var d=await r.json();
  document.getElementById('st').textContent=d.ok?'Salvato! Riavvio…':'Errore: '+d.error;
}
</script>
</body>
</html>
)rawliteral";
