  
function Slider(root,config ) {
   var knob = root.getElementsByClassName("knob")[0];
   var plate = root.getElementsByClassName("bounds")[0];
   var scale = root.getElementsByClassName("scale")[0];
   var readout = root.getElementsByClassName("readout")[0];
   var valueText = readout.getElementsByTagName("text")[0];
   var everything = root.getElementsByClassName("orient")[0];

   var bbox = scale.getBBox();
   var kbox = knob.getBBox();
   var minPos = bbox.y+bbox.height-kbox.height/2+kbox.height/4;
   var maxPos = bbox.y+kbox.height/2-kbox.height/4;   

   var value = parseInt(config.init);
   var minValue = parseInt(config.min);
   var maxValue = parseInt(config.max);
   var numDivs = parseInt(config.numDivs);
   var numSubDivs = parseInt(config.numSubDivs);
   var tickLength = parseInt(config.tickLength);
   var subTickLength = parseInt(config.subTickLength);
   var snapToTicks = parseInt(config.snapToTicks);
   var fadeOut = parseInt(config.fadeOut);
   var valuePosition = parseInt(config.valuePosition);
   var labelPosition = parseInt(config.labelPosition);
   var orientation = parseInt(config.orientation);

   var range = maxValue - minValue;
   var numTicks = numDivs * numSubDivs;      
   var pageStep = range / numDivs;
   var numDecimalPlaces = ((range) % numTicks == 0) ? 0 : (snapToTicks ? 1 : 2); 
   var bounds = plate.getBBox();
   
   var mouseDownPos = 0;
   var mouseDownValue = 0;

   setHotSpot(this, plate);
   setHotSpot(this, knob);
   setHotSpot(this,scale);
      
   this.reconfigure = function (newInstance) {
      if (newInstance) {
         config.orientation = 0;
         config.tickColour = '#000000';
         config.valueFont = getFontStyle(valueText.style); 
         config.labelFont = getFontStyle(valueText.style); 
         config.tickDis = bounds.width/2;
      }

      switch (orientation) {
         case 0 : everything.setAttribute('transform', 'rotate(0)'); break;
         case 1 : everything.setAttribute('transform', 'rotate(90)'); break;
      }
      
      
      createTicks();
      createLabels();
      
      // Position the value Text
      var pbox = plate.getBBox();
      setFontStyle(valueText.style, config.valueFont);
      switch (valuePosition + orientation * 3) {
         case 1 : valueText.setAttribute('y', pbox.y);             valueText.setAttribute('dy', '-1ex');  break;
         case 2 : valueText.setAttribute('y', pbox.y+pbox.height); valueText.setAttribute('dy', '+4ex'); break;
         case 4 : valueText.setAttribute('x', pbox.y+pbox.height); valueText.setAttribute('dx', '+0.5em'); valueText.style.textAnchor='start'; break;
         case 5 : valueText.setAttribute('x', pbox.y);             valueText.setAttribute('dx', '-0.5em'); valueText.style.textAnchor='end'; break;
      }
      switch (orientation) {
         case 0 : 
            valueText.setAttribute('transform','rotate(0)');
            valueText.setAttribute('x', '0'); 
            valueText.style.textAnchor='middle'
            break;
         
         case 1 : 
            valueText.setAttribute('y', '0'); 
            valueText.setAttribute('transform','rotate(-90)');
            valueText.setAttribute('dy', '1.25ex');
            break;
      }
      valueText.style.display = valuePosition ? "inherit" : "none";

      $(readout).stop();
      if (fadeOut) 
         $(readout).hide();
      else
         $(readout).show();

      this.setValue(value);
      
   }   
   
   this.onmousedown = function (evt, element) {
      var p = getEventPos(evt, scale);  
      var bounds = plate.getBBox();    
          
      if (element == knob) {
         // Clicked on the knob itself.
         mouseDownPos = p.y;
         mouseDownValue = value;
      }
      else if (p.y - bounds.height/2 < sliderPos(value)) {
         // Clicked above the knob, page up
         this.setValue(value + pageStep);       
      } else if (p.y - bounds.height/2 > sliderPos(value)) {
         // Clicked below the knob, page down:
         this.setValue(value - pageStep);         
      }
      
      // Jquery animation
      $(readout).stop(true, true);
      $(readout).show();
   }
   
          
   this.onmousemove = function(evt, element) { 
      if (element == knob) {
         // Knob is dragged - compute the distance as an offset in scale/slider coords and then update the value:
         var p = getEventPos(evt, scale);         
         var dif = p.y - mouseDownPos;
         this.setValue(mouseDownValue + dif / (maxPos-minPos) * (maxValue - minValue));
      }
   };

   this.onmouseup = function(e) {
      // Update the server with the final value:
      postState(root.id, "value", parseFloat(value.toFixed(numDecimalPlaces)));
      
      // Jquery animation   
      if (fadeOut)
         $(readout).fadeOut(2000);
   }; 
   
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
       
      knob.setAttribute("transform", "translate(0 " + ((sliderPos(value))-(bbox.y)) + ")");
      valueText.textContent = value.toFixed(numDecimalPlaces);

      if (capture == null && fadeOut) {
         $(readout).show();
         $(readout).fadeOut(2000);
       }
    }
    
   this.getValue = function() {
      return value;
    }

  
   function createTicks() {
      // Remove previous scale:
      var scale = root.getElementsByClassName('graticule')[0];
      var ticks = scale.getElementsByClassName('tick');
      var bounds = plate.getBBox();
      while (ticks.length > 0)
         scale.removeChild(ticks[0]);
      
      // create ticks as paths and append them to the scale.
      var step = (maxPos - minPos) / numDivs;
      var subStep = step / numSubDivs;
                 
      for (var i=0; i<=numDivs; i++) {
         var y = bbox.y + bbox.height/2 +  minPos + step * i; // n.b. minPos is an offset.
    
         for (var j=0; j<=numSubDivs; j++) {
           var len = (j==0 ? tickLength : subTickLength);
           var stroke = (j==0 && y == sliderPos(0) ? 3 : 1.5);
            
            // Create ticks on both sides.
            // TBD Hard coded '20' may need adjustment or be a config settings.
            for (var k=-1; k<=1; k+=2) {
               var tick = document.createElementNS(svgNS, "path");
               tick.setAttribute("class", "tick");
               tick.setAttribute("d", "m " + (config.tickDis) * -k + "," + y + " " + len * k  + "," + 0);
               tick.style.strokeWidth = stroke;
               tick.style.stroke = config.tickColour;
               scale.appendChild(tick);
            }
            
            if (i == numDivs)
               break;
            y += subStep;         
         }
      }
   }
   
   function createLabels () {
      // Remove previous labels
      var scale = root.getElementsByClassName('graticule')[0];
      var labels = scale.getElementsByClassName('label');
      var bounds = plate.getBBox();
      while (labels.length > 0)
         scale.removeChild(labels[0]);      
      
      if (labelPosition != 0) {
         var valueStart = minValue;
         var valueStep = (maxValue-minValue) / numDivs;
         var labelPrecision = (range % numDivs == 0) ? 0 : 1;
         var step = (maxPos - minPos) / numDivs;
                    
         for (var i=0; i<=numDivs; i++) {
            var label = document.createElementNS(svgNS, 'text');
            var value = valueStart + valueStep * i;
            var Distance = parseInt(config.labelDis);
            setFontStyle(label.style, config.labelFont);         
            switch (labelPosition+orientation*3) {
               case 1 : label.setAttribute('x', bbox.x- Distance); label.setAttribute('dx','-0.5em'); label.style.textAnchor = "end"; break;
               case 2 : label.setAttribute('x', bbox.x + bbox.width + Distance); label.setAttribute('dx','+0.5em'); label.style.textAnchor = "start"; break;
               case 4 : label.setAttribute('y', bbox.x- Distance); label.setAttribute('dy','-2ex');  break;
               case 5 : label.setAttribute('y', bbox.x + bbox.width + (Distance + 5)); label.setAttribute('dy','+4ex');  break;
            }
            if (orientation == 0) {
               label.setAttribute('y', bbox.y + bbox.height/2 + minPos + step * i);
               label.setAttribute('transform', 'rotate(0)');
               label.setAttribute('dy', '1.25ex');
            } else  {
               label.setAttribute('x', bbox.y + bbox.height/2 + maxPos - step * i);
               label.setAttribute('transform', 'rotate(-90)');
               label.style.textAnchor = "middle";
            }
            
            label.setAttribute('class', 'label');
            label.style.display='inherit';           
            label.textContent = value.toFixed(labelPrecision);   
            scale.appendChild(label);
         }                     
      }      
   }

   // Return the slider position in scale coordinates for a value v.
   function sliderPos (v) {
      return bbox.y + sliderOffset(v);
   }

   // Return the slider positional offset for a specified value v.
   // This is returned as an offset from the centre position, since the control template
   // is drawn with the knob at the centre position.
   function sliderOffset (v) {
      return minPos + (v - minValue) / (maxValue - minValue) * (maxPos - minPos);
   }
      
      
 }
  