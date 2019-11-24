char* indexPage = "\
<html>\
<head>\
<meta name=\"viewport\" content=\"height=device-height,width=device-width,initial-scale=1.0,maximum-scale=1.0\" />\
<style>\
div,fieldset,input,select{padding:5px;font-size:1em;}fieldset{background:#f2f2f2;}p{margin:0.5em 0;}input{width:100%;box-sizing:border-box;-webkit-box-sizing:border-box;-moz-box-sizing:border-box;background:#ffffff;color:#000000;}input[type=checkbox],input[type=radio]{width:1em;margin-right:6px;vertical-align:-1px;}select{width:100%;background:#ffffff;color:#000000;}textarea{resize:none;width:98%;height:318px;padding:5px;overflow:auto;background:#ffffff;color:#000000;}body{text-align:center;font-family:verdana,sans-serif;background:#ffffff;}td{padding:0px;}button{border:0;border-radius:0.3rem;background:#1fa3ec;color:#ffffff;line-height:2.4rem;font-size:1.2rem;width:100%;-webkit-transition-duration:0.4s;transition-duration:0.4s;cursor:pointer;}button:hover{background:#0e70a4;}.bred{background:#d43535;}.bred:hover{background:#931f1f;}.bgrn{background:#47c266;}.bgrn:hover{background:#5aaf6f;}a{text-decoration:none;}.p{float:left;text-align:left;}.q{float:right;text-align:right;}\
</style>\
<script>\
var MAX_WIFI_LEN = 50;\
var modeNames = [\"Chase\", \"Rainbow\", \"Double Peaks\", \"Off\"];\
var modeElem, speedElem, speedFinderElem;\
var connectionStatus = 1;\
function setClassEnabled(className, isEnabled){\
    var elems = document.getElementsByClassName(className);\
    for(var i=0; i<elems.length; i++){\
        elems[i].disabled = !(isEnabled);\
    };\
}\
function updateState(state){\
var parts = state.split(',');\
modeElem.innerHTML=modeNames[parts[0]];\
speedElem.innerHTML=parts[1];\
speedFinderElem.value=parts[1];\
if(1 == parts[2]){\
setClassEnabled('wifi_input', false);\
}else{\
setClassEnabled('wifi_input', true);\
}\
if(2 == parts[2]){\
document.getElementById('apDetails').style.display = 'none';\
} else{\
document.getElementById('apDetails').style.display = '';\
}\
}\
function doConnect(){\
   var apName = document.getElementById(\"WifiName\").value;\
   var apPassword = document.getElementById(\"WifiPassword\").value;\
   console.log('Connecting to wifi');\
   if(apName.length <1 || apPassword.length < 1){\
      alert(\"WiFi Name or password too short\");\
      return;\
   }else if(apName.length > MAX_WIFI_LEN || apPassword > MAX_WIFI_LEN) {\
      alert(\"WiFi Name or password too long\");\
      return;\
   }\
   var x = new XMLHttpRequest();\
   x.onload = function(){console.log(\"resp\" + this.response); updateState(this.response);};\
   x.open('GET', '/connect?apName='+apName+'&apPassword='+apPassword);\
   x.send()\
}\
function setMode(modeNum){\
var x = new XMLHttpRequest();\
x.onload = function(){console.log(\"resp\" + this.response); updateState(this.response);};\
x.open('GET', '/setMode?mode='+modeNum);\
x.send()\
}\
function setSpeed(modeNum){\
var x = new XMLHttpRequest();\
x.onload = function(){console.log(\"resp\" + this.response); updateState(this.response);};\
x.open('GET', '/setSpeed?speed='+Math.floor(speedFinderElem.value));\
x.send()\
}\
function init(){\
modeElem = document.getElementById('modeName');\
speedElem = document.getElementById('speed');\
speedFinderElem = document.getElementById('speedFinder');\
updateState('*TREE_STATE*');\
}\
</script>\
</head>\
<body onload=\"init()\">\
<h1>WiFi Christmas Tree</h1>\
<p>Current Mode:<span id=\"modeName\"></span></p>\
<button style=\"width:25%\" onclick=\"setMode(0)\">Chase</button><button style=\"width:25%\" onclick=\"setMode(1)\">Rainbow</button><button style=\"width:25%\" onclick=\"setMode(2)\">Double Peaks</button><button style=\"width:25%\" onclick=\"setMode(3)\">Off</button>\
<p>Current Speed:<span id=\"speed\"></span></p>\
<input id=\"speedFinder\" type=\"range\" onmouseup=\"setSpeed()\" ontouchend=\"setSpeed()\" min=\"0\" max=\"100\"/>\
<div id='apDetails'>\
<p>WiFi Name: <input id=\"WifiName\" class=\"wifi_input\" type='text'></p>\
<p>WiFi Password: <input id=\"WifiPassword\" class = \"wifi_input\" type='text'></p>\
<p><button class = \"wifi_input\" onclick=\"doConnect()\">Connect</button></p>\
</div>\
</body>\
</html>\
";
