
function PanelMeter(root, config) {
   var obj = this;
   var scale = root.getElementsByClassName('scale')[0];
   var scaleArc = scale.getElementsByTagName('path')[0];
   var needle = root.getElementsByClassName('needle')[0];
   var labelText = root.getElementsByClassName('label')[0];
   
   var minValue = parseFloat(config.min);
   var maxValue = parseFloat(config.max);   
   var numDivs = parseInt(config.numDivs);
   var numSubDivs = parseInt(config.numSubDivs);
   var tickLength = parseFloat(config.tickLength); 
   var subTickLength = parseFloat(config.subTickLength);
   var showTicks = parseInt(config.showTicks);
   var showLabels = parseInt(config.showLabels);
   
   var numTicks = numDivs * numSubDivs;
   var range = maxValue - minValue;   
   var valuePrecision = ((maxValue - minValue) % numTicks == 0) ? 0 : 2;

   // Use the endpoints of the scale path to compute the minimum and maximum angles:
   // The scale path must be an unclosed path running left to right, typically it will be an arc.
   var l = scaleArc.getTotalLength();
   var p1 = scaleArc.getPointAtLength(0);
   var p2 = scaleArc.getPointAtLength(l);
   var hdx = (p2.x-p1.x)/2;
   var arcRadius = Math.sqrt(p1.x*p1.x+p1.y*p1.y);
   
   if(p1.y < 0){
   var amin = -Math.asin(hdx/arcRadius);
   var amax = +Math.asin(hdx/arcRadius);
   } else {
   var amin = -Math.acos(hdx/arcRadius) - Math.atan(1)*2;
   var amax = +Math.acos(hdx/arcRadius) + Math.atan(1)*2;}
   
   
   // Needle animation variables
   var curValue = minValue; // Current value of the needle as it is moving during the animation
   var targetValue; // Target value of the needle - where it is heading for and should settle 
                    // (what the user has set using setValue() )
   var velocity = 0; // Current velocity of the needle
   var tmrAnim = null; // Timer / interval used for the animation callbacks

   // Needle animation parameters
   var useAnim = parseInt(config.animateNeedle); // Enable or disable animation
   var intAnim = 50; // Animation interval (milliseconds)
   var accelFactor = 1 / 1000; // How fast the needle accelerates
   var frictFactor = 300 / 1000; // Resistance slowing the needle down
   var targetStickyness = 10 / 100000; // The needle will eventually jump or stick to the target value, 
                                      // this determines how quickly that happens

   labelText.style.pointerEvents = "none";   
   
   this.reconfigure = function (newInstance) {      
      // Default font configuration is taken from valueText
      if (newInstance) {
         if (config.label == undefined)
            config.label = labelText.textContent;
         config.scaleColour = scaleArc.style.stroke;
         config.labelFont = getFontStyle(labelText.style); 
         config.scaleFont = getFontStyle(labelText.style); 
         config.scaleFont.size = 0.75*config.labelFont.size;
      }      
      
      // Set the initial value - this sets the position of the rotor in the svg so only 
      // needs to be done at config time.
      setFontStyle(labelText.style, config.labelFont);                     

      // Create the ticks and labels as required:
      createTicks();
      createLabels(); 
      //curValue = minValue; // Already done above at curValue declaration
      this.setValue(minValue);
      
      labelText.textContent = config.label;
      scaleArc.style.stroke = config.scaleColour;
   
      t = labelText;    
      c = config;         
   }

   function setAngleByValue(value) { 
      // Calculate and apply the needle angle (rotation transform) from the set value
      var angle = amin + (value - minValue) / (maxValue - minValue) * (amax - amin);
      angle = angle*180/Math.PI;
      needle.setAttribute("transform", "rotate(" + angle + ")");                 
   }

   function updateAngle() { // Update the needle angle using curValue
     setAngleByValue(curValue);
   }

   function animUpdate() { // Update the needle animation
     // Calculate how far the needle is from the destination,
     // scaling by range to bring the value into the range of 0-1
     // (so that the animation calibration values can be used consistently
     //  accross different ranges)
     var dist = (targetValue - curValue) / range;
     // Calculate and apply the needle acceleration (change in velocity)
     var dV = (dist * accelFactor) - (velocity * frictFactor);
     velocity += dV;
     //console.log("Dist: " + dist + ", Velocity: " + velocity);
     // Calculate the new needle position using the calculated change in velocity, 
     // taking into account "stickyness" (which causes the needle to jump to the target value 
     // and end the animation
     if ((Math.abs(dist) < (targetStickyness)) && 
         (Math.abs(velocity) < (targetStickyness))) {
       // The needle is close enough to the destination (target) and also moving slowly, 
       // so jump it to the final value and end the animation
       curValue = targetValue;
       velocity = 0;
       clearInterval(tmrAnim);
       tmrAnim = null;
     } else {
       // Calculate the new needle position using the change in velocity,
       // also scaling it back into the proper range
       curValue += ((velocity * intAnim) * range);
     }
     updateAngle();
   }
    
   this.setValue = function(v) {
      value = parseFloat(v);
      if (value < minValue)
         value = minValue;
      else if (value > maxValue)
         value = maxValue;

      targetValue = value;
      if (useAnim) {
        if (!tmrAnim) {tmrAnim = setInterval(function() {animUpdate();}, intAnim);}
      } else {
        curValue = targetValue;
        updateAngle();
      }
            
   }
    
   this.setLabel = function(text) {
      labelText.textContent = text;
   }
         
   function createTicks () {
      // Remove previous ticks:
      var ticks = root.getElementsByClassName('tick');
      while (ticks.length > 0)
         scale.removeChild(ticks[0]);
      
      if (showTicks) {
         // Create ticks as paths and append them to the document
         var radius = arcRadius;
         var angleStep = (amax - amin) / numDivs;
         var angleSubStep = angleStep / numSubDivs;
         var angleStart = amin;
         for (var i=0; i<=numDivs; i++) {
            var angle = angleStart + angleStep * i;
       
            for (var j=0; j<numSubDivs; j++) {
               var x = Math.sin(angle);
               var y = -Math.cos(angle);
               var len = (j==0 ? tickLength : subTickLength);
               
               var tick = document.createElementNS(svgNS, "path");            
               tick.style.fill = 'none';
               tick.style.stroke = config.scaleColour;
               tick.style.strokeWidth = '1.5px';
               tick.setAttribute('class', 'tick');
               tick.setAttribute("d", "m " +
                  (x * radius) + "," + 
                  (y * radius) + " " +
                  (x * len) + "," +
                  (y * len));
               scale.appendChild(tick);
               
               if (i == numDivs)
                  break;
               angle += angleSubStep;
            }
         }      
      }
   }    

   function createLabels () {
      // Remove previous labels:
      var labels = scale.getElementsByClassName('value');
      while (labels.length > 0)
         scale.removeChild(labels[0]);
      
      if (showLabels) {
         // Create labels based on the font settings of the value text (or possibly a font config) and append them to the document
         var radius = arcRadius + 2.0 + (showTicks ? tickLength : 0);
         var angleStart = amin;
         var angleStep = (amax - amin) / numDivs;
         var valueStart = minValue;
         var valueStep = (maxValue-minValue) / numDivs;
         var labelPrecision = (range % numDivs == 0) ? 0 : 1;
         for (var i=0; i<=numDivs; i++) {
            var label = labelText.cloneNode(true);
            var angle = angleStart + angleStep * i;
            var value = valueStart + valueStep * i;
            label.setAttribute('class', 'value');
            label.style.dominantBaseline="central";
            setFontStyle(label.style, config.scaleFont);         
            label.textContent = value.toFixed(labelPrecision);
            scale.appendChild(label);
   
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
   
   return this;
      
};

