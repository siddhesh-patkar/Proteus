function Dial (root, config) {
   var obj = this;
   var stator = root.getElementsByClassName('stator')[0];
   var rotor = root.getElementsByClassName('rotor')[0];
   var readout = root.getElementsByClassName('readout')[0];
   var valueText = readout.getElementsByTagName('text')[0];
   
   if (stator === undefined)
      stator = rotor;
        
   
   var value = parseFloat(config.init);
   var minValue = parseFloat(config.min);
   var maxValue = parseFloat(config.max);   
   var numDivs = parseInt(config.numDivs);
   var numSubDivs = parseInt(config.numSubDivs);
   var tickLength = parseFloat(config.tickLength); 
   var subTickLength = parseFloat(config.subTickLength);
   var amin = parseFloat(config.amin);
   var amax = parseFloat(config.amax);
   var snapToTicks = parseInt(config.snapToTicks);
   var showTicks = parseInt(config.showTicks);
   var showLabels = parseInt(config.showLabels);
   var fadeOut = parseInt(config.fadeOut);

   
   var numTicks = numDivs * numSubDivs;
   var range = maxValue - minValue;   
   var valuePrecision = ((maxValue - minValue) % numTicks == 0) ? 0 : (snapToTicks ? 1 : 2);
   
   valueText.style.pointerEvents = "none";   

   $(readout).stop();
   if (fadeOut) 
      $(readout).hide();
   else
      $(readout).show();
         
   setHotSpot(this, stator);   
   
   this.reconfigure = function (newInstance) {      
      // Default font configuration is taken from valueText
      if (newInstance) {
         config.tickColour = '#000000';
         config.valueFont = getFontStyle(valueText.style); 
         config.labelFont = getFontStyle(valueText.style); 
      }      
      
      // Set the initial value - this sets the position of the rotor in the svg so only 
      // needs to be done at config time.
      setFontStyle(valueText.style, config.valueFont);                     
      this.setValue(value);

      // Create the ticks and labels as required:
      createTicks();
      createLabels();      
   }
   
   // If not running the editor we need mouse handlers:
   // JKJ: rather doubt we should be hooking mousemove/mouseup directly - we probably need framework capture support for this.   
   this.onmousedown = function (e) {          
      $(readout).stop(true, true);
      $(readout).show();
   }
   
   this.onmousemove = function (e) {
      var p = getEventPos(e, stator);         
      var dist = Math.sqrt(p.x*p.x + p.y*p.y);
      if (dist > 10) {
         var angle = Math.atan2(p.x, -p.y)*180/Math.PI;
         var value = (angle-amin)/(amax-amin);
         if (value < 0)
            value = 0;
         else if (value > 1)
            value > 1;   
        obj.setValue(minValue+value*(maxValue-minValue));
      }
   }
   
   this.onmouseup = function (e) {
      postState(obj.id, "value", parseFloat(value.toFixed(valuePrecision)));
      if (fadeOut)
         $(readout).fadeOut();      
   }     
 
   this.setValue = function(v) {
      value = parseFloat(v);
      if (value < minValue)
         value = minValue;
      else if (value > maxValue)
         value = maxValue;
      
      if (snapToTicks)
       { var t = Math.round((value - minValue) / (maxValue - minValue) * numTicks);
         value = minValue + t * (maxValue - minValue) / numTicks;
       }
      
      var knobAngle = amin + (value - minValue) / (maxValue - minValue) * (amax - amin);
      rotor.setAttribute("transform", "rotate(" + knobAngle + ")");      
      valueText.textContent = value.toFixed(valuePrecision);
      
      // Jquery animation here:
      if (capture == null && fadeOut) {
         $(readout).show();
         $(readout).fadeOut(2000);
       }
    }
    
   this.getValue = function() {
      return value;
    }
      
   function createTicks () {
      // Remove previous ticks:
      var ticks = root.getElementsByClassName('tick');
      while (ticks.length > 0)
         root.removeChild(ticks[0]);
      
      if (showTicks) {
         // Create ticks as paths and append them to the document
         var bounds = stator.getBBox();
         var radius = bounds.width/2 + 5.0;
         var angleStep = (amax - amin) / (180.0 * numDivs) * Math.PI;
         var angleSubStep = angleStep / numSubDivs;
         var angleStart = (amin / 180.0) * Math.PI;
         for (var i=0; i<=numDivs; i++) {
            var angle = angleStart + angleStep * i;
       
            for (var j=0; j<numSubDivs; j++) {
               var x = Math.sin(angle);
               var y = -Math.cos(angle);
               var len = (j==0 ? tickLength : subTickLength);
               
               var tick = document.createElementNS(svgNS, "path");            
               tick.style.fill = 'none';
               tick.style.stroke = config.tickColour;
               tick.style.strokeWidth = '1.5px';
               tick.setAttribute('class', 'tick');
               tick.setAttribute("d", "m " +
                  (x * radius) + "," + 
                  (y * radius) + " " +
                  (x * len) + "," +
                  (y * len));
               root.appendChild(tick);
               
               if (i == numDivs)
                  break;
               angle += angleSubStep;
            }
         }      
      }
   }    

   function createLabels () {
      // Remove previous labels:
      var labels = root.getElementsByClassName('label');
      while (labels.length > 0)
         root.removeChild(labels[0]);
      
      if (showLabels) {
         // Create labels based on the font settings of the value text (or possibly a font config) and append them to the document
         var bounds = stator.getBBox();
         var radius = bounds.width/2 + 10.0 + (showTicks ? tickLength : 0);
         var angleStart = (amin / 180.0) * Math.PI;
         var angleStep = (amax - amin) / (180.0 * numDivs) * Math.PI;
         var valueStart = minValue;
         var valueStep = (maxValue-minValue) / numDivs;
         var labelPrecision = (range % numDivs == 0) ? 0 : 1;
         for (var i=0; i<=numDivs; i++) {
            var label = valueText.cloneNode(true);
            var angle = angleStart + angleStep * i;
            var value = valueStart + valueStep * i;
            label.setAttribute('class', 'label');
            label.style.dominantBaseline="central";
            setFontStyle(label.style, config.labelFont);         
            label.textContent = value.toFixed(labelPrecision);
            root.appendChild(label);
   
            // TBD, attempt to offset for size of the text box 
            var w = label.getBBox().width;
            var h = label.getBBox().height * 0.6;
            var x = (radius+w/2)*Math.sin(angle);
            var y = (radius+h/2)*-Math.cos(angle); 
            label.setAttribute('x', x);
            label.setAttribute('y', y);
         }             
      }                       
   }
   
}

