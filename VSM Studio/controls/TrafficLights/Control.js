function TrafficLights (root, config) {  
   var lamps = root.getElementsByTagName("ellipse");
   var red = lamps[0];
   var amber = lamps[1];
   var green = lamps[2];
   
   this.reconfigure = function (newinstance) {
      if (newinstance) {
         config.red.off = red.style.fill;
         config.amber.off = amber.style.fill;
         config.green.off = green.style.fill;
      }
      red.style.fill   = config.red.off;
      amber.style.fill = config.amber.off;
      green.style.fill = config.green.off;
      red.style.fillOpacity = 1.0;
      amber.style.fillOpacity = 1.0;
      green.style.fillOpacity = 1.0;      
   }

   this.setRed   = function (s) { red.style.fill   = s ?  config.red.on   : config.red.off; }
   this.setAmber = function (s) { amber.style.fill = s ?  config.amber.on : config.amber.off; }
   this.setGreen = function (s) { green.style.fill = s ?  config.green.on : config.green.off; }
   
   this.setState = function (s) {     
      red.style.fill = (s & 1) ?  config.red.on : config.red.off;
      amber.style.fill = (s & 2) ?  config.amber.on : config.amber.off;
      green.style.fill = (s & 4) ? config.green.on : config.green.off;
   }   
}
