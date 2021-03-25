
function Timer(root, config) {
   
   var obj = this
   
   var bounds = root.getElementsByClassName('bounds')[0]; 
   var hands = root.getElementsByClassName('hand');
   var digits = root.getElementsByClassName('digit');
   var alarm = root.getElementsByClassName('alarm')[0]; 
   var label = root.getElementsByClassName('label')[0];
   var segments = root.getElementsByClassName('segments')[0];
   
   var hour = 0, minute = 0, second = 0;
   var enableAlarm = parseInt(config.enableAlarm);
     
   this.reconfigure = function (newInstance) {
      
      if (newInstance) {     
         config.segColour = segments.style.fill;
         config.label = label.textContent;
      }

     alarm.style.opacity = enableAlarm ? 1.0 : 0.1;
     segments.style.fill = config.segColour;
     label.textContent = config.label;
     
     if (parseInt(config.dir) > 0)
        hour = minute = second = 0;
     else {
        hour =  parseInt(config.periodHour);
        minute = parseInt(config.periodMin);
        second = parseInt(config.periodSec);   
     }
      
     updateGraphics()
   }
  
   this.sync = function (h, m, s, a) {
      hour = h, minute = m, second = s;
      alarm.style.opacity = a ? 1.0 : 0.1;
      updateGraphics()
   }
   
   this.trigger = function () {
      var blink=setInterval(function() { alarm.style.opacity = 0.1; setTimeout(function() { alarm.style.opacity=1.0; }, 500); }, 1000);
      setTimeout(function() { alarm.style.opacity=0.1; clearInterval(blink)}, 5000);
      playAlarm();
   }     
  
   function updateGraphics() {
      if (hands.length > 0) {
         var cx = bounds.getBBox().width/2 + bounds.getBBox().x;
         var cy = bounds.getBBox().height/2 + bounds.getBBox().y;
         var hourHand = hands[0];
         var minuteHand = hands[1];
         var secondHand = hands[2];
         secondHand.setAttribute("transform", "rotate(" + second*6 + " " + cx + " "  + cy + ")");
         minuteHand.setAttribute("transform", "rotate(" + (minute+second/60)*6 + " " + cx + " "  + cy + ")");
         hourHand.setAttribute("transform", "rotate(" + (hour+minute/60)*6 + " " + cx + " "  + cy + ")");
      } else if (digits.length > 0) {
          var seconds = second < 10?"0"+second.toString():second.toString();
          var minutes = minute < 10?"0"+minute.toString():minute.toString();
          var hours   = hour < 10?"0"+hour.toString():hour.toString();
         
          clearDigits();
          setDigit(0,seconds[0]);
          setDigit(1,seconds[1]);
          setDigit(2,minutes[0]);
          setDigit(3,minutes[1]);
          setDigit(4,hours[0]);
          setDigit(5,hours[1]);
      }
   }
  
   
   function clearDigits(){
      for (var i=0; i<=5; i++) {
         for (var l=0; l<=6; l++) {
            digits[i].getElementsByTagName("polygon")[l].style.opacity = 0.1;
            digits[i].getElementsByTagName("polygon")[l].style.fill = config.segColour;
         }
      }
   }
   
   function setDigit(digit,value){
      var segments = digits[digit].getElementsByTagName("polygon");
      
      switch(value){
         case "0":
            segments[0].style.opacity = 1
            segments[1].style.opacity = 1
            segments[2].style.opacity = 1
            segments[3].style.opacity = 1
            segments[4].style.opacity = 1
            segments[5].style.opacity = 1
            break;       
                         
         case "1":       
            segments[1].style.opacity = 1
            segments[2].style.opacity = 1
            break;       
                         
         case "2":       
            segments[0].style.opacity = 1
            segments[1].style.opacity = 1
            segments[3].style.opacity = 1
            segments[4].style.opacity = 1
            segments[6].style.opacity = 1
            break;       
                         
         case "3":       
            segments[0].style.opacity = 1
            segments[1].style.opacity = 1
            segments[2].style.opacity = 1
            segments[3].style.opacity = 1
            segments[6].style.opacity = 1
            break;       
                         
         case "4":       
            segments[1].style.opacity = 1
            segments[2].style.opacity = 1
            segments[5].style.opacity = 1
            segments[6].style.opacity = 1
            break;       
                         
         case "5":       
            segments[0].style.opacity = 1
            segments[2].style.opacity = 1
            segments[3].style.opacity = 1
            segments[5].style.opacity = 1
            segments[6].style.opacity = 1
            break;       
                         
         case "6":       
            segments[0].style.opacity = 1
            segments[2].style.opacity = 1
            segments[3].style.opacity = 1
            segments[4].style.opacity = 1
            segments[5].style.opacity = 1
            segments[6].style.opacity = 1
            break;       
                         
         case "7":       
            segments[0].style.opacity = 1
            segments[1].style.opacity = 1
            segments[2].style.opacity = 1
            break;       
                         
         case "8":       
            segments[0].style.opacity = 1
            segments[1].style.opacity = 1
            segments[2].style.opacity = 1
            segments[3].style.opacity = 1
            segments[4].style.opacity = 1
            segments[5].style.opacity = 1
            segments[6].style.opacity = 1
            break;       
                         
         case "9":       
            segments[0].style.opacity = 1
            segments[1].style.opacity = 1
            segments[2].style.opacity = 1
            segments[5].style.opacity = 1
            segments[6].style.opacity = 1
            break;
      }     
      
      
   }
}
  
