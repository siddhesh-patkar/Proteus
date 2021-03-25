function RotarySwitch(root, config) { 
   var obj = this;
   var stator = root.getElementsByClassName('stator')[0];
   var rotor = root.getElementsByClassName('rotor')[0];
   var readout = root.getElementsByClassName('readout')[0];
   var Labels = root.getElementsByClassName('lables')[0];
   var valueText = readout.getElementsByTagName('text')[0];

   if (stator === undefined)
      stator = rotor;    
   
   
   var value = parseInt(config.init);
   var numDivs = parseInt(config.numStates)-1; 
   var tickLength = parseFloat(config.tickLength); 
   var amin = parseFloat(config.amin);
   var amax = parseFloat(config.amax);
   var showTicks = parseInt(config.showTicks);
   var showLabels = parseInt(config.showLabels);
	
   setHotSpot(this, stator); 
   setHotSpot(this, Labels);  
   
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
                         
      this.setState(value);
      // Create the ticks and labels as required:
      createTicks();
      createLabels();    
      
   }
   
   this.onmousedown = function (e) {          
      this.onmousemove(e);
   }
   
   this.onmousemove = function (e) {
      var p = getEventPos(e, stator);     
      var dist = Math.sqrt(p.x*p.x + p.y*p.y);
      if (dist > 10) {
         var angle = Math.atan2(p.x, -p.y)*180/Math.PI;
         var newvalue = (angle-amin)/(amax-amin);
         if (newvalue < 0)
            newvalue = 0;
         else if (newvalue > 1)
            newvalue = 1;   
        newvalue = Math.round(newvalue*numDivs);
        if (newvalue != value) {
           obj.setState(newvalue);
           buttonClick();
        }
      }
   }
   
   this.onmouseup = function (e) {
      postState(obj.id, "state", parseFloat(value.toFixed(0))); 
   }     
  
   //This sets the angle of the rotor 
   this.setState = function(v) {
      value = parseInt(v);
      if (value < 0)
         value = 0;
      else if (value > numDivs)
         value = numDivs;
                   
      var knobAngle = amin + value  / numDivs * (amax - amin);
      rotor.setAttribute("transform", "rotate(" + knobAngle + ")");         
    }
   
   this.getState = function() {
      return  value;
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
         var angleSubStep = angleStep;
         var angleStart = (amin / 180.0) * Math.PI;
         for (var i=0; i<=numDivs; i++) {
           var angle = angleStart + angleStep * i;
       
           var x = Math.sin(angle);
           var y = -Math.cos(angle);
           var len =  tickLength;  
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


   function createLabels () {
      // Remove previous labels:
      var labels = root.getElementsByClassName('label');
      while (labels.length > 0)
         Labels.removeChild(labels[0]);
      
      if (showLabels) {
         // Create labels based on the font settings of the value text (or possibly a font config) and append them to the document
         var bounds = stator.getBBox();
         var radius = bounds.width/2 + 10.0 + (showTicks ? tickLength : 0);
         var angleStart = (amin / 180.0) * Math.PI;
         var angleStep = (amax - amin) / (180.0 * numDivs) * Math.PI;
         
         var labelText = [];
         if (config.tickLabels != undefined)
         labelText = config.tickLabels.split(',');
      
         for (var i=0; i<=numDivs; i++) {
            var label = valueText.cloneNode(true);
            var angle = angleStart + angleStep * i;
            label.setAttribute('class', 'label');
            label.style.dominantBaseline="central";
            setFontStyle(label.style, config.labelFont); 
            if (labelText[i]!= undefined){
            	label.textContent = labelText[i];
              Labels.appendChild(label);}else{
            	label.textContent = i + 1
            	Labels.appendChild(label);}            	    
   
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

