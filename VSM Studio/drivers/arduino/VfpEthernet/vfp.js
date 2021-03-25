   // Virtual Front Panel - Javascript Framework.

var controlObjects = [];
var statusFails = 0;
var statusAbort = false;
 
function initPage() {
   window.onbeforeunload = function() { statusAbort = true; };       
   
   if (typeof(popup) == "object") {
      // Simulation mode: cheat by loading the panel directly:
      var content = popup.loadLibrary("panel.svg");
      var div = document.createElement("div");
      div.innerHTML = content;               
      var container = document.getElementById("container");                                                            
      var panelRoot = div.getElementsByTagName("svg")[0]
      panelRoot.setAttribute("class", "panel");                                    
      container.replaceChild(panelRoot, container.firstChild);
      initPanel(panelRoot);
      statusRequest();  
   } else {
      var xhttp = new XMLHttpRequest();
      xhttp.onreadystatechange = function() {
         if (xhttp.readyState == 4) {
            if (xhttp.status == 200) {
               var container = document.getElementById("container");                                    
               var panelDoc = xhttp.responseXML; 
               var panelRoot = panelDoc.documentElement;
               panelRoot.setAttribute("class", "panel");                                    
               container.replaceChild(panelRoot, container.firstChild);
               initPanel(panelRoot);
               statusRequest(); 
            }
            else if (xhttp.status == 404) {
               var container = document.getElementById("container");                     
               container.innerHTML = "<P class=\"error\">PANEL.SVG not found - please refer to the documentation</P>";                     
            }
             
         }
      };
      xhttp.open("GET", "/panel.svg", true);
      xhttp.send(); 
   }                        
}

function initPanel (svg) {
   var layerNodes = svg.getElementsByTagName("g");
   if (typeof(popup) == "object") {
      var panelRect = svg.getBoundingClientRect();
      popup.resize(panelRect.width+50, panelRect.height+50);
   }            
   for (var i=0; i<layerNodes.length; i++) {
      var controlNodes = layerNodes[i].getElementsByTagName("g");
      for (var j=0; j<controlNodes.length; j++) {
         if (controlNodes[j].hasAttribute("class")) {
            var className = controlNodes[j].getAttribute("class");
            console.log("Found control class "+className);
            if (eval("typeof "+className+" == \"function\"")) {
               var control = eval(" new "+className+"(controlNodes[j])");
               control.id = controlNodes[j].id;
               console.log("Created "+className+ " object with id", control.id);
               controlObjects[control.id]=control;
            }
         }
      }
   }                                         
}

function statusRequest() {
  var xhttp = new XMLHttpRequest();
  xhttp.timeout = 0;
  xhttp.onreadystatechange = function() {
    if (xhttp.readyState == 4 && !statusAbort) {
      if (xhttp.status == 200) {
         parseResponse(xhttp.responseText);
         statusFails = 0;
         statusRequest(); 
     }
     else if (statusFails < 30) {
         // If the status request fails then try to re-make it every second.
         statusFails += 1;
         window.setTimeout(statusRequest, 1000); 
     }
     else {
        // Failed to reconnect - show an error:
        var container = document.getElementById("container");                     
        container.innerHTML = "<P class=\"error\">Lost connection to VFP Server</P>";                     
     }

       
    }
  };                  
  xhttp.open("GET", "status", true);
  xhttp.send();
}

function postMessage (id, msg) {
   var xhttp = new XMLHttpRequest();
   var params = "";
   xhttp.open("POST", id, true);
   xhttp.setRequestHeader("Content-type", "application/x-www-form-urlencoded");
   if (msg === undefined)
      xhttp.send();
   else {
      if (typeof(msg)!='Array')
         msg = [ msg ];               
      for (var i in msg) { 
         if (typeof(msg[i]) === 'string')
            msg[i] = JSON.stringify(msg[i]);
         if (i > 0)
            params = params+',';   
         params = params + msg[i];
      }
      xhttp.send(params+'\n');
   }
}

function parseResponse (responseText) {
   var lines = responseText.split('\n');
   for (var i in lines) {
      if (lines[i].length > 0) {
         console.log(lines[i]);                     
         var rx = /([$a-zA-Z_][a-zA-Z0-9_]*)(.*)/;
         var parts = rx.exec(lines[i]);
         var command;
         if (parts != null && parts.length == 3) {
            if (parts[1].slice(0,1)=="$") {
               switch(parts[1].substr(1).toLowerCase()) {
                  case "title" :  command = "document.title" + parts[2]; break;
               }                        
            }
            else if (typeof controlObjects[parts[1]] == "object") {
               command = "controlObjects[\""+parts[1]+"\"]"+parts[2];
            }
            if (typeof command === "string")   
               eval(command);
         }
      }
   }
}

// Create a JS canvas element over a control and return its 2d context
function createCanvas (ctrl) {
   var panel = document.getElementById("container");
   var panelRect = panel.getBoundingClientRect();
   var ctrlRect = ctrl.getElementsByTagName("rect")[0].getBoundingClientRect();
   var top = ctrlRect.top - panelRect.top;
   var bottom = ctrlRect.bottom - panelRect.top;
   var left = ctrlRect.left - panelRect.left;
   var right = ctrlRect.right - panelRect.top;
   var width = ctrlRect.right - ctrlRect.left;
   var height = ctrlRect.bottom - ctrlRect.top;

   var canvas = document.createElement("canvas");
   canvas.className="canvas";
   canvas.width = width;
   canvas.height = height;
   canvas.style.top = top+"px";
   canvas.style.left = left+"px";
   panel.appendChild(canvas);
   return canvas.getContext("2d");
}

// Convert CSV to an array of string values. Returns NULL if CSV string not well formed.
function CSVtoArray (text) {
    var re_valid = /^\s*(?:'[^'\\]*(?:\\[\S\s][^'\\]*)*'|"[^"\\]*(?:\\[\S\s][^"\\]*)*"|[^,'"\s\\]*(?:\s+[^,'"\s\\]+)*)\s*(?:,\s*(?:'[^'\\]*(?:\\[\S\s][^'\\]*)*'|"[^"\\]*(?:\\[\S\s][^"\\]*)*"|[^,'"\s\\]*(?:\s+[^,'"\s\\]+)*)\s*)*$/;
    var re_value = /(?!\s*$)\s*(?:'([^'\\]*(?:\\[\S\s][^'\\]*)*)'|"([^"\\]*(?:\\[\S\s][^"\\]*)*)"|([^,'"\s\\]*(?:\s+[^,'"\s\\]+)*))\s*(?:,|$)/g;
    // Return NULL if input string is not well formed CSV string.
    if (!re_valid.test(text)) return null;
    var a = [];                     // Initialize array to receive values.
    text.replace(re_value, // "Walk" the string using replace with callback.
        function(m0, m1, m2, m3) {
            // Remove backslash from \' in single quoted values.
            if      (m1 !== undefined) a.push(m1.replace(/\\'/g, "'"));
            // Remove backslash from \" in double quoted values.
            else if (m2 !== undefined) a.push(m2.replace(/\\"/g, '"'));
            else if (m3 !== undefined) a.push(m3);
            return ''; // Return empty string.
        });
    // Handle special case of empty last value.
    if (/,\s*$/.test(text)) a.push('');
    return a;
}
