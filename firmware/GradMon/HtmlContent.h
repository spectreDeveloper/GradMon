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
.result{display:flex;align-items:center;gap:12px;background:#1a1a1a;border:1px solid #222;border-radius:10px;padding:10px 12px;margin-top:8px;cursor:pointer;transition:.15s}
.result:hover{border-color:#3a3a3a}
.result.sel{border-color:#f5c518;background:#141200}
.result .res-img-container{width:46px;height:64px;flex-shrink:0;position:relative}
.result img,.result .img-ph{width:100%;height:100%;object-fit:contain;border-radius:5px;background:#111}
.r-info{flex:1;min-width:0}
.r-name{font-size:13px;font-weight:600;color:#eee;line-height:1.3;white-space:nowrap;overflow:hidden;text-overflow:ellipsis}
.r-sub{font-size:11px;color:#666;margin-top:2px;white-space:nowrap;overflow:hidden;text-overflow:ellipsis}
.r-badge{display:inline-block;background:#222;border-radius:4px;padding:2px 6px;font-size:10px;color:#888;margin-top:4px;margin-right:3px}
.r-price-tag{float:right;color:#f5c518;font-weight:700;font-size:13px;margin-top:2px}
.card-detail{display:flex;gap:14px;margin-bottom:16px}
.card-detail img{width:90px;height:126px;object-fit:contain;border-radius:8px;background:#111;flex-shrink:0;box-shadow:0 4px 20px #0008}
.card-detail .info{flex:1;min-width:0}
.detail-name{font-size:17px;font-weight:700;color:#fff;line-height:1.2;margin-bottom:6px}
.price-grid{display:grid;grid-template-columns:repeat(2,1fr);gap:8px;margin-bottom:10px}
.price-opt{background:#1a1a1a;border:1px solid #222;border-radius:10px;padding:10px 12px;cursor:pointer;transition:.15s;position:relative}
.price-opt.sel{border-color:#f5c518;background:#141200}
.price-opt.sel::after{content:'✓';position:absolute;top:7px;right:9px;color:#f5c518;font-size:11px;font-weight:700}
.p-group{font-size:9px;color:#555;text-transform:uppercase;letter-spacing:1.5px;margin-bottom:3px}
.p-label{font-size:12px;color:#bbb;font-weight:600;margin-bottom:5px}
.p-val{font-size:20px;color:#f5c518;font-weight:800;letter-spacing:-0.5px}
.oled-frame{width:256px;height:128px;background:#000;border:2px solid #111;border-radius:4px;margin:0 auto;overflow:hidden;display:flex;flex-direction:column;padding:4px 6px 3px}
.ol{font-family:'Courier New',monospace;overflow:hidden;white-space:nowrap;color:#5aff5a}
.ol-sm{font-size:12px;line-height:1.45}
.ol-lg{font-size:18px;font-weight:700;line-height:1.2}
.ol-hr{border:none;border-top:1px solid #0a2a0a;margin:3px 0}
.ol-dhr{border:none;border-top:2px solid #0a2a0a;margin:2px 0}
.ol-row{display:flex;justify-content:space-between;align-items:baseline}
.ol-boot{font-size:11px;color:#2a5a2a;text-align:center;padding-top:38px;font-family:monospace;letter-spacing:1px}
.hidden{display:none}
</style>
</head>
<body>
<div class="hdr"><h1>GRADMON</h1><p>Smart Graded Card Display</p></div>
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
<div class="card">
  <div class="card-title">Cerca carta</div>
  <input type="text" id="searchQ" placeholder="es. Charizard, Pikachu V, Blastoise EX…" onkeydown="if(event.key==='Enter')doSearch()">
  <button class="btn btn-dim" onclick="doSearch()">Cerca</button>
  <div id="results"></div>
</div>
<div class="card hidden" id="detailCard">
  <div class="card-title" id="detailTitle">Seleziona prezzo primario</div>
  <div class="card-detail">
    <img id="cardImg" src="" alt="" onerror="this.style.display='none'">
    <div class="info">
      <div class="detail-name" id="dName"></div>
      <div class="detail-sub" id="dSet"></div>
      <div class="detail-sub" id="dNum"></div>
      <div class="detail-badges" id="dBadges"></div>
    </div>
  </div>
  <div class="card-title">Opzioni prezzo disponibili</div>
  <div class="price-grid" id="priceGrid"></div>
  <div class="card-title" style="margin-top:20px">Layout Display OLED</div>
  <select id="oledLayout" onchange="updateOled()">
    <option value="0">Classico (Dettagliato)</option>
    <option value="1">Prezzo Grande (Centrato)</option>
    <option value="2">Minimal (Nome + Prezzo)</option>
    <option value="3">Immagine + Info</option>
    <option value="4">Lista Prezzi (Titolo + 4 Prezzi)</option>
  </select>
  <button class="btn btn-green hidden" id="sendBtn" onclick="sendToDisplay()">▶ Invia al display OLED</button>
  <div class="status" id="sendSt"></div>
</div>
<div class="card">
  <div class="card-title">Preview display OLED</div>
  <div class="oled-frame" id="oledFrame"><div class="ol-boot">&#9673; GRADMON v1.0<br><br>Pronto.</div></div>
</div>
<script>
var CARDS=[], SEL=null, SEL_PRICE=null;
var CURRENCY='EUR';
var SYM={EUR:'€',USD:'$',GBP:'£',JPY:'¥'};
function sym(c){return SYM[c]||c;}
function fmt(v,c){
  if(!v||isNaN(v))return null;
  v=parseFloat(v);
  return sym(c)+' '+(v%1===0?v.toLocaleString('it-IT'):v.toLocaleString('it-IT',{minimumFractionDigits:2,maximumFractionDigits:2}));
}
function trunc(s,n){if(!s)return'';s=String(s);return s.length<=n?s:s.slice(0,n-1)+'~';}
function esc(s){return String(s||'').replace(/&/g,'&amp;').replace(/</g,'&lt;').replace(/>/g,'&gt;');}
function setSt(id,msg,cls){var e=document.getElementById(id);e.textContent=msg;e.className='status '+(cls||'');}
async function saveConfig(){
  var k=document.getElementById('apiKey').value.trim();
  CURRENCY=document.getElementById('currency').value;
  if(!k){setSt('cfgSt','Inserisci la API key','err');return;}
  setSt('cfgSt','Salvataggio…','info');
  var r=await fetch('/api/config',{method:'POST',headers:{'Content-Type':'application/json'},body:JSON.stringify({apiKey:k,currency:CURRENCY})});
  var d=await r.json();
  setSt('cfgSt',d.ok?'Salvato!':'Errore: '+d.error,d.ok?'ok':'err');
}
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
    var pb = (c.priceOptions && c.priceOptions.length > 0) ? '<div class="r-price-tag">' + fmt(c.priceOptions[0].value, c.priceOptions[0].currency||CURRENCY) + '</div>' : '';
    html+='<div class="result" id="ri'+i+'" onclick="pick('+i+')"><div class="res-img-container">';
    if(c.imageUrl) html+='<img src="'+esc(c.imageUrl)+'" loading="lazy" onerror="this.style.display=\'none\';this.nextElementSibling.style.display=\'flex\'"><div class="img-ph" style="display:none">&#129655;</div>';
    else html+='<div class="img-ph">&#129655;</div>';
    html+='</div><div class="r-info">'+pb+'<div class="r-name">'+esc(c.nameNum||c.name)+'</div><div class="r-sub">'+esc(c.setName||'—')+(c.cardNumber?' &nbsp;#'+c.cardNumber:'')+'</div></div></div>';
  });
  document.getElementById('results').innerHTML=html;
}
function pick(i){
  document.querySelectorAll('.result').forEach(function(e){e.classList.remove('sel');});
  document.getElementById('ri'+i).classList.add('sel');
  SEL=CARDS[i]; SEL_PRICE=null;
  document.getElementById('detailTitle').textContent=SEL.name;
  document.getElementById('dName').textContent=SEL.nameNum||SEL.name;
  document.getElementById('dSet').textContent=SEL.setName||'Set sconosciuto';
  document.getElementById('dNum').textContent=SEL.cardNumber?'#'+SEL.cardNumber+' — '+SEL.supertype:'';
  var img=document.getElementById('cardImg');
  if(SEL.imageUrl){img.src=SEL.imageUrl;img.style.display='';} else img.style.display='none';
  var badges=(SEL.rarity?'<span class="badge rare">'+esc(SEL.rarity)+'</span>':'')+(SEL.hp?'<span class="badge hp">HP '+SEL.hp+'</span>':'');
  document.getElementById('dBadges').innerHTML=badges;
  buildPriceGrid(SEL.priceOptions||[]);
  document.getElementById('detailCard').classList.remove('hidden');
  document.getElementById('sendBtn').classList.add('hidden');
  updateOled();
}
function buildPriceGrid(opts){
  var html='';
  opts.forEach(function(o,i){
    html+='<div class="price-opt" id="po'+i+'" onclick="selPrice('+i+')"><div class="p-group">'+esc(o.group)+'</div><div class="p-label">'+esc(o.label)+'</div><div class="p-val">'+esc(fmt(o.value,o.currency||CURRENCY))+'</div></div>';
  });
  document.getElementById('priceGrid').innerHTML=html||'<div class="no-prices">Nessun prezzo</div>';
}
function selPrice(i){
  document.querySelectorAll('.price-opt').forEach(function(e){e.classList.remove('sel');});
  document.getElementById('po'+i).classList.add('sel');
  var opt=SEL.priceOptions[i];
  SEL_PRICE={ key:opt.key, label:opt.label, group:opt.group, value:opt.value, currency:opt.currency||CURRENCY, formatted:fmt(opt.value,opt.currency||CURRENCY) };
  document.getElementById('sendBtn').classList.remove('hidden');
  updateOled();
}
async function sendToDisplay(){
  if(!SEL||!SEL_PRICE) return;
  setSt('sendSt','Elaborazione…','info');
  var imgHex = '';
  if(SEL.imageUrl) try { imgHex = await processImage(SEL.imageUrl); } catch(e){ console.error(e); }
  var extraPs=[];
  if(SEL.priceOptions){
    SEL.priceOptions.slice(0, 5).forEach(o=>{
      var lbl=(o.group&&o.group!=='Cardmarket'&&o.group!=='TCGPlayer')?o.group.substring(0,3)+' '+o.label:o.label.substring(0,8);
      extraPs.push(lbl+': '+fmt(o.value,o.currency||CURRENCY));
    });
  }
  var payload={
    name:SEL.name, setName:SEL.setName||'', cardNumber:SEL.cardNumber||'', 
    grade:(SEL_PRICE.group&&SEL_PRICE.group!=='Cardmarket'&&SEL_PRICE.group!=='TCGPlayer')?SEL_PRICE.group+' '+SEL_PRICE.label:SEL_PRICE.label,
    price:SEL_PRICE.formatted, cardId:SEL.id, priceKey:SEL_PRICE.key,
    layout:parseInt(document.getElementById('oledLayout').value),
    image:imgHex, p2:extraPs[1]||'', p3:extraPs[2]||'', p4:extraPs[3]||''
  };
  var r=await fetch('/api/display',{method:'POST',headers:{'Content-Type':'application/json'},body:JSON.stringify(payload)});
  var d=await r.json();
  setSt('sendSt',d.ok?'✓ Inviato!':'Errore: '+d.error,d.ok?'ok':'err');
}
function updateOled(){
  var f=document.getElementById('oledFrame');
  if(!SEL){f.innerHTML='<div class="ol-boot">Pronto.</div>';return;}
  var name=trunc(SEL.nameNum||SEL.name, 21), layout=document.getElementById('oledLayout').value;
  var grade='', price='';
  if(SEL_PRICE){ grade=trunc((SEL_PRICE.group&&SEL_PRICE.group!=='Cardmarket'&&SEL_PRICE.group!=='TCGPlayer')?SEL_PRICE.group+' '+SEL_PRICE.label:SEL_PRICE.label,12); price=trunc(SEL_PRICE.formatted,11); }
  if(layout=="4"){
    var html='<div class="ol ol-sm" style="text-align:center">'+esc(name)+'</div><div class="ol-hr"></div>';
    if(SEL.priceOptions) SEL.priceOptions.slice(0,4).forEach(o=>html+='<div class="ol ol-sm">'+esc(o.label)+': '+esc(fmt(o.value,o.currency||CURRENCY))+'</div>');
    f.innerHTML=html;
  } else if(layout=="3"){
    f.innerHTML='<div style="display:flex;gap:10px">'+(SEL.imageUrl?'<img src="'+esc(SEL.imageUrl)+'" style="width:40px;height:56px;object-fit:contain;filter:grayscale(1) contrast(2)">':'<div style="width:40px;height:56px;border:1px solid #2a6a2a"></div>')+'<div style="flex:1"><div class="ol ol-sm">'+esc(name)+'</div><div class="ol ol-lg" style="font-size:16px">'+esc(price)+'</div><div class="ol ol-sm">'+esc(grade)+'</div></div></div>';
  } else {
    f.innerHTML='<div class="ol ol-sm">'+esc(name)+'</div><div class="ol-dhr"></div><div class="ol-row"><div class="ol ol-lg">'+esc(grade)+'</div><div class="ol ol-lg">'+esc(price)+'</div></div>';
  }
}
function processImage(url){
  return new Promise((resolve,reject)=>{
    var img=new Image(); img.crossOrigin="Anonymous";
    img.onload=function(){
      var canvas=document.createElement('canvas'); canvas.width=40; canvas.height=56;
      var ctx=canvas.getContext('2d'); ctx.fillStyle="white"; ctx.fillRect(0,0,40,56);
      var r=img.width/img.height, w=40, h=56;
      if(r > 40/56) h=40/r; else w=56*r;
      ctx.drawImage(img,(40-w)/2,(56-h)/2,w,h);
      var imgData=ctx.getImageData(0,0,40,56), d=imgData.data, gray=new Float32Array(40*56);
      for(var i=0; i<d.length; i+=4){ var v=(d[i]*0.21+d[i+1]*0.72+d[i+2]*0.07); v=(v-128)*1.3+128; gray[i/4]=Math.max(0,Math.min(255,v)); }
      var bits=new Uint8Array(280);
      for(var y=0; y<56; y++) for(var x=0; x<40; x++){
        var i=y*40+x, oldV=gray[i], newV=oldV<128?0:255, err=(oldV-newV)/8;
        if(newV===0) bits[y*5+Math.floor(x/8)]|=(1<<(x%8));
        if(x+1<40) gray[i+1]+=err; if(x+2<40) gray[i+2]+=err;
        if(y+1<56){ if(x>0) gray[i+40-1]+=err; gray[i+40]+=err; if(x+1<40) gray[i+40+1]+=err; }
        if(y+2<56) gray[i+80]+=err;
      }
      var hex=""; for(var b of bits) hex+=b.toString(16).padStart(2,'0');
      resolve(hex);
    };
    img.onerror=reject; img.src=url;
  });
}
fetch('/api/status').then(r=>r.json()).then(d=>{
  if(d.currency){CURRENCY=d.currency;document.getElementById('currency').value=d.currency;}
  if(d.layout!==undefined){document.getElementById('oledLayout').value=d.layout;}
}).catch(()=>{});
</script>
</body>
</html>
)rawliteral";

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
  var s=document.getElementById('ssid').value.trim(), p=document.getElementById('pass').value;
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
