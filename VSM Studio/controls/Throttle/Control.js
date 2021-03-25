
  
function Throttle(root,config ) {
   //var orient = root.firstElementChild;
   var obj = this;
   var knob = root.getElementsByClassName("knob")[0];
   var track = root.getElementsByClassName("track")[0];

   var bbox = track.getBBox();
   var kbox = knob.getBBox();
   var minPos = bbox.y+bbox.height;
   var maxPos = bbox.y;

   var value = parseInt(config.init);
   var minValue = parseInt(config.min);
   var maxValue = parseInt(config.max);
   var orientation = parseInt(config.orientation);

   var range = maxValue - minValue;
   
   var mouseDownPos = 0;
   var mouseDownValue = 0;
   
   var updateTimer = setInterval(postValue, 1000/parseFloat(config.rate));
   var updateValue = value;

   setHotSpot(this, knob);
      
   this.reconfigure = function (newInstance) {
        if (newInstance) {
           config.init = 0;
           config.minValue = 0;
           config.maxValue = 255;          
        }
      this.setValue(config.init);
      
   }   
   
   this.onmousedown = function (evt, element) {
      var p = getEventPos(evt, track);         
      if (element == knob) {
         // Clicked on the knob itself.
         mouseDownPos = p.y;
         mouseDownValue = value;
      }
     
   }
   
          
   this.onmousemove = function(evt, element) { 
      if (element == knob) {
         // Knob is dragged - compute the distance as an offset in scale/slider coords and then update the value:
         var p = getEventPos(evt, track);         
         var dif = p.y - mouseDownPos;
         this.setValue(mouseDownValue + dif / (maxPos-minPos) * range);
         
      
      }
   };

   this.onmouseup = function(e) {
      
     
   }; 
   
   this.setValue = function(v) {
      value = parseInt(v);
      if (value < minValue)
         value = minValue;
      else if (value > maxValue)
         value = maxValue;
     knob.setAttribute("transform", "translate(0 " + sliderPos(value) + ")");
      
  //    valueText.textContent = value.toFixed(numDecimalPlaces);
    }
    
   this.getValue = function() {
      return value;
    }

   function postValue () {
      if (document.getElementById(obj.id) == null)
         clearInterval(updateTimer);
      else if (updateValue != value)
         postState(obj.id, "value", value);
         updateValue = value;
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
      
  
   return this;          
 }
  