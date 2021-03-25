function SlideSwitch (root,config ) {
   var orient = root.firstElementChild;
   var knob = root.getElementsByClassName("knob")[0];
   var plate = root.getElementsByClassName("bounds")[0];
   var scale = root.getElementsByClassName("scale")[0];
   var readout = root.getElementsByClassName("readout")[0];
   var valueText = readout.getElementsByTagName("text")[0];
   var graticule = root.getElementsByClassName('graticule')[0];

   var bbox = scale.getBBox();
   var kbox = knob.getBBox();
   var boundryBox = plate.getBBox();
   var minPos = bbox.y+bbox.height-kbox.height/4;
   var maxPos = bbox.y+kbox.height/4;   

   var value = parseInt(config.init);
   var minValue = 0;
   var numTicks = parseInt(config.numStates)-1;
   var maxValue = parseInt(numTicks);
   var tickLength = parseInt(config.tickLength);
   var labelPosition = parseInt(config.labelPosition);
   var orientation = parseInt(config.orientation);
   var stagger = parseInt(config.staggeredLabels);
   var textSize = parseInt(config.labelFont.size);
   
   var range = maxValue - minValue;
   var pageStep = range / numTicks;
   
   var mouseDownPos = 0;
   var mouseDownValue = 0;

	valueText.setAttribute("display","none")

   setHotSpot(this, plate);
   setHotSpot(this, knob);
      
   this.reconfigure = function (newInstance) {
      if (newInstance) {
         config.orientation = 0;
         config.tickColour = '#000000';
         config.valueFont = getFontStyle(valueText.style); 
         config.labelFont = getFontStyle(valueText.style); 
      }

      switch (orientation) {
         case 0 : orient.setAttribute('transform', 'rotate(0)'); break;
         case 1 : orient.setAttribute('transform', 'rotate(90)'); break;
      }
      
      
      createTicks();
      createLabels();
      
      this.setState(value);
      
   }   
   
   this.onmousedown = function (evt, element) {
      var p = getEventPos(evt, scale);  
      
      var h = p.y - bbox.height/2
             
      if (element == knob) {
         // Clicked on the knob itself.
         mouseDownPos = p.y;
         mouseDownValue = value;         
      }
      else if (h < sliderPos(value)) {
         // Clicked above the knob, page up
         this.setState(value + pageStep);   
         buttonClick();
      } else if (h > sliderPos(value)) {
         // Clicked below the knob, page down:
         this.setState(value - pageStep);         
         buttonClick();
      }
   }
   
          
   this.onmousemove = function(evt, element) { 
      if (element == knob) {
         // Knob is dragged - compute the distance as an offset in scale/slider coords and then update the value:
         var p = getEventPos(evt, scale);         
         var dif = p.y - mouseDownPos;
         var newState = mouseDownValue + Math.round(dif / (maxPos-minPos) * (maxValue - minValue)); 
         if (newState != value) {
            this.setState(newState);
            buttonClick();
         }
      }
   };

   this.onmouseup = function(e) {
      // Update the server with the final value:
      postState(root.id, "state", parseFloat(value.toFixed(1)));
      
   }; 
   ////////////////////////////////////////////////////////
   this.setState = function(v) {
      value = parseInt(v);
      if (value < minValue)
         value = minValue;
      else if (value > maxValue)
         value = maxValue;


        var t = Math.round((value - minValue) / (maxValue - minValue) * numTicks);
        value = minValue + t * (maxValue - minValue) / numTicks;
       
       
      knob.setAttribute("transform", "translate(0 " + (sliderPos(value) - bbox.y) + ")");
      valueText.textContent = value.toFixed(1);
    }
   /////////////////////////////////////////////////////////
    
   this.getState = function() {
      return value;
    }

  
   function createTicks() {
      // Remove previous scale:
      var scale = root.getElementsByClassName('graticule')[0];
      var ticks = scale.getElementsByClassName('tick');
      while (ticks.length > 0)
         scale.removeChild(ticks[0]);
      
      // create ticks as paths and append them to the scale.
      var step = (maxPos - minPos) / numTicks;
      var subStep = step;
                 
      for (var i=0; i<=numTicks; i++) {
         var y = bbox.y + bbox.height/2 +  minPos + step * i; // n.b. minPos is an offset.
    
         for (var j=0; j<=1; j++) {
           var len = (j==0 ? tickLength : 1);
           var stroke = (j==0 && y == sliderPos(0) ? 3 : 1.5);
            
            // Create ticks on both sides.
            // TBD Hard coded '20' may need adjustment or be a config settings.
            for (var k=1; k<=1; k+=1) {
            	
            	if(labelPosition+orientation*3 == 2 || labelPosition+orientation*3 == 5){            		
            		var c = k
            	}else{
            		var c = -k
            		}
            	
               var tick = document.createElementNS(svgNS, "path");
               tick.setAttribute("class", "tick");
               tick.setAttribute("d", "m " + ((boundryBox.width/2 +4) * c) + "," + y + " " + len * c  + "," + 0);
               tick.style.strokeWidth = stroke;
               tick.style.stroke = config.tickColour;
               scale.appendChild(tick);
            }                
         }
      }
   }
   
   function createLabels () {
      // Remove previous labels
      var scale = root.getElementsByClassName('graticule')[0];
      var labels = scale.getElementsByClassName('label');
      while (labels.length > 0)
         scale.removeChild(labels[0]);      
      
      if (labelPosition != 0) {
         var valueStart = minValue;
         var valueStep = (maxValue-minValue) / numTicks;
         var labelPrecision = (range % numTicks == 0) ? 0 : 1;
         var step = (maxPos - minPos) / numTicks;
         
         var labelText = [];
         if (config.labels != undefined)
         labelText = config.labels.split(',');
                    
         for (var i=0; i<=numTicks; i++) {
            var label = document.createElementNS(svgNS, 'text');
            var value = valueStart + valueStep * i;
            setFontStyle(label.style, config.labelFont);
            label.textContent = i
            
            var offSet = 0
            
            if (i % 2 != 0 & orientation !=0 & stagger == 1){
            offSet = textSize + 5}
            
            
 
            switch (labelPosition+orientation*3) {
               case 1 : label.setAttribute('x', boundryBox.x - tickLength); label.setAttribute('dx','-0.5em'); label.style.textAnchor = "end"; break;
               case 2 : label.setAttribute('x', boundryBox.x + boundryBox.width + tickLength); label.setAttribute('dx','+0.5em'); label.style.textAnchor = "start"; break;
               case 4 : label.setAttribute('y', boundryBox.x - offSet - tickLength); label.setAttribute('dy','-2ex');  break;
               case 5 : label.setAttribute('y', boundryBox.x + boundryBox.width + offSet + tickLength); label.setAttribute('dy','+4ex');  break;
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
            
            if (labelText[i] != undefined){
            label.textContent = labelText[i]}else{
            label.textContent = i
            }
            
            label.setAttribute('class', 'label');
            label.style.display='inherit';           
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
  