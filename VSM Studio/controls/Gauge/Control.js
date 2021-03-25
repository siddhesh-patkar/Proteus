  
function Gauge(root,config ) {
   var indicator = root.getElementsByClassName("indicator")[0];
   var plate = root.getElementsByClassName("bounds")[0];
   var scale = root.getElementsByClassName("scale")[0];
   var readout = root.getElementsByClassName("readout")[0];
   var valueText = readout.getElementsByTagName("text")[0];
   var everything = root.getElementsByClassName("orient")[0];
   var colour = root.getElementsByClassName("colour")[0];

   var bbox = scale.getBBox();
   var minPos = bbox.y+bbox.height;
   var maxPos = bbox.y;   

   var value = parseFloat(config.init);
   var minValue = parseInt(config.min);
   var maxValue = parseInt(config.max);
   var numDivs = parseInt(config.numDivs);
   var numSubDivs = parseInt(config.numSubDivs);
   var tickLength = parseInt(config.tickLength);
   var subTickLength = parseInt(config.subTickLength);
   var labelPosition = parseInt(config.labelPosition);
   var orientation = parseInt(config.orientation);

   var range = maxValue - minValue;
   var numTicks = numDivs * numSubDivs;      
   var bounds = plate.getBBox();
   
   // Bar animation variables
   var curValue = minValue; // Current value of the bar as it is moving during the animation
   var targetValue; // Target value of the bar - where it is heading for and should settle 
                    // (what the user has set using setValue() )
   var velocity = 0; // Current velocity of the bar
   var tmrAnim = null; // Timer / interval used for the animation callbacks
   var useAnim = parseInt(config.animateGauge); // Enable or disable animation

   valueText.setAttribute("display","none")
   
   
   this.reconfigure = function (newInstance) {
   	//This grabs the pre existing styles from the SVG and puts them into the XML code,
      	//the two styles which are taken from the SVG are the label font and the LED colour.
      	//This will only run when the user drags the control from the list.
         if (newInstance) {
            if (colour!=undefined){
            config.gaugeColour = colour.style.fill;
			config.emptyColour = scale.style.fill;
           }else{
			config.emptyColour = undefined;
           	config.gaugeColour = undefined;
           }
         }   
         
         //This allows the user to set the colour of the LED. It takes the value from the XML and
         //applies it to the colour layer of the SVG.
         
         if(colour!=undefined){
			 scale.style.fill = config.emptyColour;
   			 colour.style.fill = config.gaugeColour;
				 //code to try fix the blur refresh issue
				 //By transforming the SVG in any way it forces the web browser to update and allows the colour to update as well
				 //however the image i have already has a transform on it and therefore needs to keep it to remain looking nice
				 colour.setAttribute('transform', colour.getAttribute("transform"));  
         }
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

      this.setValue(value);
      
   }
   

    function setPositionByValue(value) { 
    // Calculate and apply the bar height from the set value
      var position = ((scale.getBBox().height/range) * value)
      if (position < 0)
         position = 0;
      else if (position > scale.getBBox().height)
         position = scale.getBBox().height;
      indicator.setAttribute('y', scale.getBBox().y + scale.getBBox().height- position); // Position the bar level with the bottom of the scale
      indicator.setAttribute('height', position); // Set the bar length relative to the scale
   }
                                      
   function updatePosition() { // Update the bar length using curValue
     setPositionByValue(curValue);
   }

   function animUpdate() { // Update the bar animation
     // Calculate how far the bar is from the destination,
     // scaling by range to bring the value into the range of 0-1
     // (so that the animation calibration values can be used consistently
     //  accross different ranges)
     var dist = (targetValue - curValue) / range;
     // Calculate and apply the bar acceleration (change in velocity)
     var dV = (dist * 1/1000) - (velocity * 3/10);
     velocity += dV;
     // Calculate the new bar position using the calculated change in velocity, 
     // taking into account "stickyness" (which causes the bar to jump to the target value 
     // and end the animation
     if ((Math.abs(dist) < (1/10)) && 
         (Math.abs(velocity) < (1/10))) {
       // The bar is close enough to the destination (target) and also moving slowly, 
       // so jump it to the final value and end the animation
       curValue = targetValue;
       velocity = 0;
       clearInterval(tmrAnim);
       tmrAnim = null;
     } else {
       // Calculate the new bar position using the change in velocity,
       // also scaling it back into the proper range
       curValue += ((velocity * 50) * range);
     }
     updatePosition();
   }
   
   this.setValue = function(v) {
      value = parseFloat(v);
      if (value < minValue)
         value = minValue;
      else if (value > maxValue)
         value = maxValue;
   
      targetValue = value;
      if (useAnim) {
        if (!tmrAnim) {tmrAnim = setInterval(function() {animUpdate();}, 50);}
      } else {
        curValue = targetValue;
        updatePosition();
      }
 }
   
  
   function createTicks() {
      // Remove previous scale:
      var scale = root.getElementsByClassName('graticule')[0];
      var ticks = scale.getElementsByClassName('tick');
      while (ticks.length > 0)
         scale.removeChild(ticks[0]);
      
      // create ticks as paths and append them to the scale.
      var step = (maxPos - minPos) / numDivs;
      var subStep = step / numSubDivs;
                 
      for (var i=0; i<=numDivs; i++) {
         var y = bbox.y + bbox.height/2 +  minPos + step * i; // n.b. minPos is an offset.
    
         for (var j=0; j<=numSubDivs; j++) {
           var len = (j==0 ? tickLength : subTickLength);
           var stroke = null;
            
            // Create ticks on both sides.
            // TBD Hard coded '20' may need adjustment or be a config settings.
            for (var k=-1; k<=1; k+=1) {
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

      
 }
  