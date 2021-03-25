function Bearing (root, config) {

   var pointer = root.getElementsByClassName('pointer')[0];    
   var bounds = root.getElementsByClassName('bounds')[0];
   var value = parseFloat(config.init);
   var targetValue = 0;
   var curValue = 0;
   var velocity = 0;
   var animating = false;
 
   function animUpdate() {
     // Calculate how far the bar is from the destination,
     // scaling by range to bring the value into the range of 0-1
     // (so that the animation calibration values can be used consistently
     //  accross different ranges)
     var dist = (targetValue - curValue);
     // Calculate and apply the bar acceleration (change in velocity)
     velocity += (dist * 1/1000) - (velocity * 3/10);
     // Calculate the new bar position using the calculated change in velocity, 
     // taking into account "stickyness" (which causes the bar to jump to the target value 
     // and end the animation
     
     if ((Math.abs(dist) < (1/10)) && (Math.abs(velocity) < (1/10))){
       // The bar is close enough to the destination (target) and also moving slowly, 
       // so jump it to the final value and end the animation
       curValue = value;
       velocity = 0;
       animating = false;
       clearInterval(tmrAnim);
       tmrAnim = null;
     } else {
       // Calculate the new bar position using the change in velocity,
       // also scaling it back into the proper range
       curValue += (velocity * 50);
     }
     // Calculate and apply the rotation from the set value
     pointer.setAttribute("transform", "rotate(" + curValue + " " + (pointer.getBBox().width/2 + pointer.getBBox().x) + " "  + (pointer.getBBox().height/2 + pointer.getBBox().y) + ")");
   }
   
 
   this.setValue = function(v) {
      //checks to see if the program is still animating
      if (animating == false){
         animating = true; //sets animating to true and starts animating
         //root.getElementsByTagName("text")[0].textContent = value; // a little diagnostic tool to show numbers using the dials test
      } else {
         clearInterval(tmrAnim);
         tmrAnim = null;
      }
      value = parseFloat(v); 
      targetValue = ((curValue-value) > 180)? value+360:value;  //clockwise
      targetValue = ((value-curValue) > 180)? -360+value:targetValue; //anti clockwise
      tmrAnim = setInterval(function() {animUpdate()}, 50); //Do animation
   }
}

