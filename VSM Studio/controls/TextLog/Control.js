
function TextLog(root, config) {
   var obj = this;
   var border = root.getElementsByTagName('rect')[0];
   var bounds = root.getElementsByTagName('rect')[1];
   var textRegion = root.getElementsByTagName('g')[0];
   var textArea = textRegion.getElementsByTagName('rect')[0];
   var textSample = textRegion.getElementsByTagName('text')[0];   

   var content = createOverlay(root, textArea, 'div');
   content.style.listStyle = "none";
   content.style.textAlign = "left";
   content.style.overflowX = "hidden";
   content.style.overflowY="auto";
   content.style.outline="none";
   content.style.pointerEvents = 'none';

   if (config.textFont != undefined) {
      content.style.fontFamily = config.textFont.family;
      content.style.fontSize = config.textFont.size + 'px';
      content.style.color = config.textFont.colour;
   }

   var loadDataPending = false;
   var loadDataPendingRows = [];
   
   
   this.reconfigure = function (newInstance) {
      if (newInstance) {        
          config.textFont = getFontStyle(textSample);
          config.backgroundColour = border.style.fill;
          textSample.style.display = 'none';
      }

      border.style.fill = config.backgroundColour;
      content.style.fontFamily = config.textFont.family;
      content.style.fontSize = config.textFont.size + 'px';
      content.style.color = config.textFont.colour;
   }
   
   this.update = function () {
      // Fix the border to have fixed pixel width:
      var ctm = root.getCTM();
      var bt = panel.createSVGTransform();
      bt.setScale(1/ctm.a, 1/ctm.d);
      border.transform.baseVal.initialize(bt);
      border.width.baseVal.value = bounds.width.baseVal.value*ctm.a;
      border.height.baseVal.value = bounds.height.baseVal.value*ctm.d;         
   }
   
   this.setFile = function (filename) {            
      // Request the log file and load it into the content area:
      requestFile(filename, function (text) {
         // Process log file:
         obj.clear();
         if (text != null) {
            var lines = text.split('\r\n'); 
            for (var i=0; i<lines.length; i++) {
               if (lines[i].length > 0)               
                  appendItem(lines[i]);
            }
         }
      });     
   };
    
   this.appendLine = function (line)  {
      if (loadDataPending)
         loadDataPendingRows.push(line)
      else 
         appendItem(line);
   };
               
   this.clear = function() {
       content.innerHTML = "";
   };
   
   function appendItem (line) {
      var li = document.createElement('li');
      var text, flag;
      if (line[1] == '\r' && line[2] != '\n')
         flag = line[0], text = line.substring(2);
      else   
         text = line;
      switch (flag) {         
         case 'w' : li.style.color = config.warnColour; break;
         case 'e' : li.style.color = config.errorColour; break;
      }
      content.appendChild(li);
      li.textContent = text;
      li.scrollIntoView();
   }

   if (typeof(editor) == 'object') {
      appendItem(textSample.textContent);
      content.style.pointerEvents = 'none';
   } else
      content.style.pointerEvents = '';      
   
   return this;
}

