
function Terminal(root, config) {
   var obj = this;
   var border = root.getElementsByTagName('rect')[0];
   var bounds = root.getElementsByTagName('rect')[1];
   var textRegion = root.getElementsByTagName('g')[0];
   var textSample = textRegion.getElementsByTagName('text')[0];   
   var text = null;
   
   var inputBorder = root.getElementsByClassName('inputBorder')[0];
   var inputRegion = root.getElementsByTagName('g')[1];
   var inputArea = inputRegion.getElementsByTagName('rect')[0];
   var placeHolder = inputRegion.getElementsByTagName('text')[0];
   
   var input = null;
   
   var pastCommands = [];
   var pastCommandSelected = 0;      
   
   setHotSpot(this, inputBorder);

   var textArea = textRegion.getElementsByTagName('rect')[0];
   text = createOverlay(root, textArea, 'textarea');
   text.style.fontFamily = config.textFont.family;
   text.style.fontSize = config.textFont.size + 'px';
   text.style.color = config.textFont.colour;
   text.style.outline = 'none';
   text.onmousedown = function (e) { e.stopPropagation(); }
   text.onmouseup   = function (e) { e.stopPropagation(); }
   text.readOnly = true;

   if (typeof(editor) == 'object') {
      text.value = textSample.textContent+'!';
      text.style.pointerEvents = 'none';
   }
   textSample.style.display = 'none';

   placeHolder.style.pointerEvents='none';
   
   this.reconfigure = function (newInstance) {
      if (newInstance) {        
        config.textFont = getFontStyle(textSample);
        config.placeHolder = placeHolder.textContent;
      }
      placeHolder.textContent = config.placeHolder;
      text.style.fontFamily = config.textFont.family;
      text.style.fontSize = config.textFont.size + 'px';
      text.style.color = config.textFont.colour;
   }
   
   this.update = function () {
      var ctm = root.getCTM();
      var tbt = panel.createSVGTransform();
      tbt.setScale(1/ctm.a, 1/ctm.d);
      border.transform.baseVal.initialize(tbt);
      border.width.baseVal.value = bounds.width.baseVal.value*ctm.a;
      border.height.baseVal.value = bounds.height.baseVal.value*ctm.d;      
      
      // Compute a transform for the input border rectangle:
      var ibt = panel.createSVGTransform();
      ibt.setScale(1/ctm.a, 1/ctm.d);
      inputBorder.transform.baseVal.initialize(ibt);
      inputBorder.y.baseVal.value = border.height.baseVal.value-inputBorder.height.baseVal.value-1;
      inputBorder.width.baseVal.value = (bounds.width.baseVal.value)*ctm.a-2;

      // Adjust the height of the text area
      textArea.height.baseVal.value = bounds.height.baseVal.value - inputBorder.height.baseVal.value/ctm.d;      
      
      // Reposition the input region group:
      var irt = panel.createSVGTransform();
      irt.setTranslate(0, textArea.height.baseVal.value);
      inputRegion.transform.baseVal.initialize(irt);
      
      // Re-scale the input area rectangle to maintain constant height:
      var iat = panel.createSVGTransform();
      iat.setScale(1, 1/ctm.d);
      inputArea.transform.baseVal.initialize(iat);

            
      // Force the text objects to have 1:1 aspect ratio at the panel level by compensating for any distortion.
      var tt = panel.createSVGTransform();
      tt.setScale(1/ctm.a, 1/ctm.d);
      textSample.transform.baseVal.initialize(tt);
      var pt = panel.createSVGTransform();
      pt.setScale(1/ctm.a, 1/ctm.d);
      placeHolder.transform.baseVal.initialize(pt);          
   }
 
   this.onmousedown = function (e) {
       if (input == null) {
            beginInput();
       } else {
            endInput();
       }
   }
   
     this.onmouseup = function (e) {
        return input != null; // capture continues whilst the input element is active
   }
       
   this.startNewLine = function () {
      var lines = text.value.split('\n');
      if (lines != undefined && lines.length > 0) {
         var lastLine = lines[lines.length-1];
         if (lastLine.length != 0)
            this.appendText('\n');
      }
   }    
       
   this.appendText = function (s) {
      text.value = text.value + s;
      text.scrollTop = text.scrollHeight; 
   };
   

   this.cls = function() {
      text.value = "";
   };

   
   this.setPrompt = function(prompt) {
      placeHolder.textContent = prompt;
   }
      
     
   function beginInput () {
      if (input == null) {         
         input = createOverlay(root, inputArea, 'input');
         input.type = 'text';
         input.maxLength = config.cmdLength;
         input.style.fontFamily = placeHolder.fontFamily;
         input.style.fontSize = placeHolder.fontSize; 
         input.focus();
                                   
         // Set up event handles on the input element.
         input.onmousedown = function (e) { e.stopPropagation(); }
         input.onkeydown = function (e) {
            if (e.keyCode == 13) {
               if (pastCommands.indexOf(input.value) < 0) {
                  pastCommands.push(input.value);
                  pastCommandSelected = pastCommands.length;   
               }            
               if (input.value == config.clsCmd) {
                  postEvent("$.erase", obj.id);
                  obj.cls();
               } else {
                  postEvent(obj.id, input.value);
               }
               input.value = "";
            } else if (e.keyCode == 27) {
               endInput();       
            } else if (!e.shiftKey && !e.ctrlKey) {
               if (e.keyCode == 38 && pastCommands.length) { // UP
                  pastCommandSelected--;
                  if (pastCommandSelected >= 0)
                     input.value = pastCommands[pastCommandSelected];            
                  else  
                      pastCommandSelected = 0;                  
                  e.preventDefault();
               }
               else if (e.keyCode == 40 && pastCommands.length) { // DOWN
                  pastCommandSelected++; 
                  if (pastCommandSelected < pastCommands.length) 
                     input.value = pastCommands[pastCommandSelected];
                  else {   
                     pastCommandSelected = pastCommands.length;
                     input.value = "";
                  }
                  e.preventDefault();
               }
            }            
            placeHolder.style.display = 'none';
         }         
      }
   }
   
   function endInput () {
      if (input != null) {
         placeHolder.style.display = '';
         deleteOverlay(input);         
         capture = null;
         input = null;
      }
   }
   
   return this;
}
   

