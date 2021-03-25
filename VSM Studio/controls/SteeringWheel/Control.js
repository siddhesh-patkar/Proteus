function SteeringWheel(root, config) {
   var obj = this;
   var stator = root.getElementsByClassName('stator')[0];
   var rotor = root.getElementsByClassName('rotor')[0];
   var readout = root.getElementsByClassName('readout')[0];
   var valueText = readout.getElementsByTagName('text')[0];
         
   var value = parseInt(config.init);
   var amin = parseInt(config.amin);
   var amax = parseInt(config.amax);
   var anchorAngle, anchorValue;
         
   valueText.style.pointerEvents = "none";   
               
   var updateTimer = setInterval(postValue, 1000/parseFloat(config.rate));
   var updateValue = value;

   if (typeof(viewer) == 'object')
      viewer.registerSensorHandler(root.id, 'Accelerometer');

   setHotSpot(this, stator);            
   
   this.reconfigure = function (newInstance) {     
      if (newInstance) {
         config.init = 0; 
         config.amin = -90;
         config.amax = +90;
      }
      
      this.setValue(config.init);      
   }
   
   // If not running the editor we need mouse handlers:
   this.onmousedown = function (e) {          
      var p = getEventPos(e, stator);         
      var dist = Math.sqrt(p.x*p.x + p.y*p.y);
      if (dist > 30) {
         anchorAngle = Math.atan2(p.x, -p.y)*180/Math.PI;
         anchorValue = value;
      } else {
         obj.setValue(0);
         anchorAngle = undefined;
      }   
   }
   
   
   this.onmousemove = function (e) {
      var p = getEventPos(e, stator);         
      var dist = Math.sqrt(p.x*p.x + p.y*p.y);
      if (dist > 30 && anchorAngle != undefined) {
         var angle = Math.atan2(p.x, -p.y)*180/Math.PI;
         var newValue = angle-anchorAngle + anchorValue;
         if (Math.abs(newValue - value) > 180)
            newValue += (angle < -90) ? 360 : -360;
         if (newValue != value && newValue < 180 && newValue > -180) {
            obj.setValue(newValue.toFixed(0));
         }
      }
   }
   
   this.onmouseup = function (e) {      
   }     
 
   
   this.onsensorchanged = function (sensorType, values) {
      if (sensorType == 'Accelerometer') {
         // This will be called each time the host device has an update
         var pitch = values[1]*180/Math.PI;
         var roll = values[2]*180/Math.PI;
         if (roll < -45)
            roll = -45;
         else if (roll > 45)
            roll = 45;   
         var angle = amin + (roll + 45) / 90 * (amax-amin)
         obj.setValue(angle);         
         
         // Detach from mouse operation:
         clearHotSpot(this, stator);   
      }
   }
 
   
   this.setValue = function(v) {
      v = parseFloat(v);
      if (v < amin)
         v = amin;
      else if (v > amax)
         v = amax;         
      value = Math.round(v);
      
      rotor.setAttribute("transform", "rotate(" + v + ")");      
      valueText.textContent = value;      
            
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
   
      
   return this;
}   