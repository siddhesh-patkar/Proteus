
function LineEdit (root, config) {
   var obj = this;
   var border = root.getElementsByTagName('rect')[0];
   var bounds= root.getElementsByTagName('rect')[1];
   var clipRegion = root.getElementsByTagName('g')[0];
   var textArea = clipRegion.getElementsByTagName('rect')[0];
   var text = clipRegion.getElementsByTagName('text')[0];
   var placeHolder = clipRegion.getElementsByTagName('text')[1];
   var errorMessage = clipRegion.getElementsByTagName('text')[2];
   var input = root.input;
      
   setHotSpot(this, root);
   
   this.reconfigure = function (newInstance) {      
      if (newInstance) {
         config.textFont = getFontStyle(text);
         config.placeHolder = placeHolder.textContent;
      }
      placeHolder.textContent = config.placeHolder;
      this.setText("");
      this.setError("");
   };
   
   this.update = function () {
      // Force the border to have 1:1 aspect ratio at the panel level and retain it's stroke witdth:
      var ctm = root.getCTM();
      var bt = panel.createSVGTransform();
      bt.setScale(1/ctm.a, 1/ctm.d);
      border.transform.baseVal.initialize(bt);
      border.width.baseVal.value = bounds.width.baseVal.value*ctm.a;
      border.height.baseVal.value = bounds.height.baseVal.value*ctm.d;      
      
      // Force the text objects to have 1:1 aspect ratio at the panel level by compensating for any distortion.
      var tt = panel.createSVGTransform();
      tt.setScale(ctm.d/ctm.a, 1);
      text.transform.baseVal.initialize(tt);
      var pt = panel.createSVGTransform();
      pt.setScale(ctm.d/ctm.a, 1);
      placeHolder.transform.baseVal.initialize(pt);
      var et = panel.createSVGTransform();
      et.setScale(ctm.d/ctm.a, 1);
      errorMessage.transform.baseVal.initialize(et);
   };
   
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
    
   this.getText = function() {
      return input != null ? input.value : text.textContent;
   };

   this.setText = function(msg) {
      text.textContent = msg;
      if (input != null)
         input.value = msg;         
      placeHolder.style.display = (msg == "") ? '' : 'none'
   };

   this.clear = function() {
      text.textContent = text;
   };
   
   this.setError = function (msg) {
      errorMessage.textContent = msg;
   }      

   function beginInput () {
       if (input == null) {         
          var ctm = text.getCTM();
          input = createOverlay(root, textArea, 'input');            
          input.style.fontFamily = config.textFont.family;
          input.style.fontSize = config.textFont.size*ctm.d + 'px';
          input.value=text.textContent;
                
          input.type = 'text';
          input.readOnly = config.readOnly == 1;
          input.maxLength = config.maxLength;
          input.focus()

          textArea.style.fill = '#E0E0F0';
          text.style.display = 'none';
          placeHolder.style.display = 'none';
          errorMessage.textContent = "";          
                         
         // Set up event handles on the input element.
         input.onmousedown = function (e) { e.stopPropagation(); }
         input.onkeypress = function (e) {
            if (e.keyCode == 13) {
               text.textContent = input.value;
               postState(obj.id, "text", input.value);
               endInput();       
            } else if (e.keyCode == 27) {
               endInput();       
            }
         };
       }
   }
   
   function endInput () {
      if (input != null) {
         text.style.display = '';
         placeHolder.style.display = text.textContent == "" ? '' : 'none';
         textArea.style.fill = 'none';
         deleteOverlay(input);
         capture = null;
         input = null;
      }
   }
   
   return this;
};
