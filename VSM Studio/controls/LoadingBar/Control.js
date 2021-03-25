 function LoadingBar(root, config) {
   var scale = root.getElementsByClassName('limits')[0];
   var bounds = root.getElementsByClassName('bounds')[0];
   var bar = root.getElementsByClassName('bar')[0];
   var text = root.getElementsByTagName("text")[0];
   var min = scale.getBBox().x;
   var max = min+scale.getBBox().width;
   var bbox = bounds.getBBox();
   var tbox = text.getBBox();
   var range = max - min;
   
   // Bar animation variables
   var curValue = min; // Current value of the bar as it is moving during the animation
   var targetValue; // Target value of the bar - where it is heading for and should settle 
                    // (what the user has set using setValue() )
   var velocity = 0; // Current velocity of the bar
   var tmrAnim = null; // Timer / interval used for the animation callbacks
   var useAnim = parseInt(config.animateBar); // Enable or disable animation
   
   //setup
   this.reconfigure = function (newinstance) {
	   if (newinstance) {
	      config.textFont = getFontStyle(text.style)
	      config.barColour = bar.style.fill;
		  config.textVisible = "1"
	  }  
	  text.style.display = parseInt(config.textVisible) ? "inherit" : "none";
	  tbox = text.getBBox();
      bar.style.fill = config.barColour;
      setFontStyle(text.style, config.textFont)
      text.style.textAnchor='middle'
      text.setAttribute('y', bbox.height/2 + tbox.height/4);
      text.setAttribute('x', bbox.width/2);
	  //allows user to set an initial value
	  this.setValue(config.init)
   }
   
   
   function setPositionByValue(value) { 
    // Calculate and apply the bar height from the set value
      var end = (scale.getBBox().width/100)* value  
      	  text.textContent = parseInt(value) + "%";
	  //Changes the width of the loading bar to the current value#
	  bar.setAttribute('width', end);
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
   
   this.setValue = function(location) {
	  //checks to make sure location isnt too big or small
      value = parseFloat(location);
      if (location < 0){
	  location = 0;}
      else if (location > 100){
	  location = 100;}
    
	  targetValue = location;
      if (useAnim) {
        if (!tmrAnim) {tmrAnim = setInterval(function() {animUpdate();}, 50);}
      } else {
        curValue = targetValue;
        updatePosition();
      }
	  
    }
    
}