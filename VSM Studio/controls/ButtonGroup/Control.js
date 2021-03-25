
function ButtonGroup (root, config) {    
   var group = this;
   var elements = root.getElementsByClassName("element");
   var size = elements.length;
   var state = config.init;
   var spacing = parseInt(config.buttonSpacing);
   var faceRects = root.getElementsByClassName('buttonFace'); 

   
   // Instantiate the buttons
   var buttons = [];
   for (var i=0; i<size; ++i) {
      buttons[i] = new RadioButton(this, elements[i], config, i);
      buttons[i].setState(0);
   }

   
   this.reconfigure = function (newinstance) {
      var labels = root.getElementsByTagName("text");
      var boundRects = root.getElementsByClassName("bounds");        
      var lampRects = root.getElementsByClassName('lamp');
      if (newinstance) {
         config.labelFont = getFontStyle(labels[0].style);     
         config.lampColour = lampRects[0].style.fill;
         
         if (faceRects.length > 0)
               if (config.faceColour === undefined)
                  config.faceColour = faceRects[0].style.fill;         
      }   
      
      // Re-position and configure the buttons. 
      //var x = 0, y = 0;
      //for (var i=0; i<size; ++i) {
      //   var t = panel.createSVGTransform();
      //   t.setTranslate(x,y);
      //   elements[i].transform.baseVal.initialize(t);
      //   if (config.orientation == 0)
      //      x += boundRects[i].getBBox().width+spacing;
      //   else if (config.orientation == 1)   
      //      y += boundRects[i].getBBox().height+spacing;
      // }
      
      // Position the labels:
      var labelText = [];
      if (config.buttonLabels != undefined)
         labelText = config.buttonLabels.split(',');
      for (var i=0; i<size; ++i) {
         var label = labels[i];
         var bbox = boundRects[i].getBBox();
         label.textContent = labelText[i] != undefined ? labelText[i] : "" ;
         label.style.pointerEvents = "none";   
         setFontStyle(label.style, config.labelFont);
      
         var x = null, y = null;
         var h = label.getBBox().height * 0.6;
         var s = parseInt(config.labelSpacing);
         switch (parseInt(config.labelPosition)) {
            case 1 : label.style.textAnchor = "middle"; x = bbox.width/2; y = -s;  break;
            case 2 : label.style.textAnchor = "middle"; x = bbox.width/2; y = bbox.height+s + h; break;
            case 3 : label.style.textAnchor = "end"; x = -s; y = bbox.height/2 + h/2; break;
            case 4 : label.style.textAnchor = "start";  x = bbox.width+s; y = bbox.height/2 + h/2; break;            
         }   

         if (x != null && y != null) { 
            label.setAttribute('x', x);
            label.setAttribute('y', y); 
            label.style.display = "inherit"; 
         } else {
            label.style.display = "none"; 
         }
      }
      
      // Apply face colour to all button faces across all the elements
      for (var i = 0; i<faceRects.length; ++i)
         faceRects[i].style.fill = config.faceColour;         
      
      // This will apply attributes to the SVG so only needs to be done at config time:   
      this.setState(state);  
   };

   
   // Control Methods:
   this.setState = function (newState) {
      for (var i=0; i<buttons.length; ++i) 
         buttons[i].setLamp(newState == i);
      state = newState;                     
   }

   this.getState = function () {
      return state;
   }                  
}

// Local class object for an individual button element
// This is essentially a cut down version of class PushButton.
function RadioButton (group, root, config, idx) {
   var symbols = root.getElementsByTagName("g");      
   var lamp = root.getElementsByClassName('lamp')[0];
   
   setHotSpot(this, root);
   
   // We use style.display to avoid conflicts with inkscape
   symbols[0].removeAttribute('display');
   symbols[1].removeAttribute('display');
                  
   this.onmousedown = function (evt) {
      if (group.getState() != idx) {
         group.setState(idx);
         postState(group.id, "state", idx);  
         buttonClick();             
      }
      this.setState(true);
   };
   
   this.onmouseup = function (evt) {
      this.setState(false);
   };
              
   this.setState = function (down) {
      if (down) {
         symbols[0].style.display="none";
         symbols[1].style.display="inline"; 
      } else {
         symbols[0].style.display="inline";
         symbols[1].style.display="none";
      }         
   };   

   this.setLamp = function (flag) {
      if (lamp != undefined) {
         lamp.style.opacity = flag ? 1.0 : 0.2;
         lamp.style.fill = config.lampColour;
      }
   };

}   


