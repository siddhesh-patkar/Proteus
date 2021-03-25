 
function Thermometer(root, config) {
   var CELCIUS = 0;
   var FARENHEIT = 1
   var scale = root.getElementsByClassName('scale')[0];
   var mercury = root.getElementsByClassName('mercury')[0];
   var tmin = scale.getBBox().y;
   var tmax = tmin+scale.getBBox().height;
   
   this.setTemperature = function(temp) {
      if (config.units == FARENHEIT)
         temp = (temp - 32) * 5.0 / 9.0; // to Celcius
         
      if (temp < tmin)
         temp = tmin;
      else if (temp > tmax)
         temp = tmax;
   
      // The scaling of the mercury element in the graphics is configured so that there is a 1:1 mapping between
      // degrees C and the top of the mercury rect in y pixels.
      mercury.setAttribute('height', temp-tmin);
    }
    
}