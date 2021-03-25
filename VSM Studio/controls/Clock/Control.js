
function Clock(root, config) {
   
   var obj = this
   
   var bounds = root.getElementsByClassName('bounds')[0]; 
   var hands = root.getElementsByClassName('hand');
   var segments = root.getElementsByClassName('segments')[0];
   var digits = root.getElementsByClassName('digit');
   var alarm = root.getElementsByClassName('alarm')[0]; 
   var pm = root.getElementsByClassName('pm')[0]; 
   var label = root.getElementsByClassName('label')[0];
   var inputArea = root.getElementsByClassName('inputArea')[0];
   
   var hour = 0, minute = 0, second = 0;
   var useBrowserTime = parseInt(config.useBrowserTime);
   var mode = null;
   var input = null;

   setHotSpot(this, root);     
     
   if (useBrowserTime &&  typeof(editor) == "undefined") {
      // Set the clock from the browser time:
      var now = new Date();
      hour = now.getHours();
      minute = now.getMinutes();
      second = now.getSeconds();
      var args = [ 0, hour, minute, second ];
      postEvent(root.id, args);
      useBrowserTime = false;
   }           
     
   this.reconfigure = function (newInstance) {
      
      if (newInstance) {     
         config.label = label.textContent;
         config.labelFont = getFontStyle(label);
         config.segColour = segments.style.fill;
         if (pm != undefined)
            config.showPM = 1;
         else
            delete config.showPM;
         }

     if (pm)
        pm.style.display = parseInt(config.showPM) ? 'inline' : 'none';

     segments.style.fill = config.segColour;
     alarm.style.opacity = 0.1;
     label.textContent = config.label;
     setFontStyle(label, config.labelFont);
           
     updateGraphics()
   }
  
   this.setMode = function (mode, h, m) {
      var minutes = m < 10?"0"+m.toString():m.toString();
      var hours   = h < 10?"0"+h.toString():h.toString();
      var init = hours+":"+minutes;
      beginInput(mode, init);            
   }

   // TBD - this function should cancel the setting mode.
   this.cancelMode = function () {
      endInput();
   }
  
   this.sync = function (h, m, s, a) {
      if (input != null)
         return; // Ignore sync whilst setting.
      if (useBrowserTime) {
         // Get the time from the browser and pass it back to the client as though
         // the time had been set though the GUI. This works as a one time operation for each session.
      }
      hour = h, minute = m, second = s;
      alarm.style.opacity = a ? 1.0 : 0.1;
      updateGraphics()
   }
   
   this.trigger = function () {
      // TBD make the alarm indicator flash:      
      var blink=setInterval(function() { alarm.style.opacity = 0.1; setTimeout(function() { alarm.style.opacity=1.0; }, 500); }, 1000);
      setTimeout(function() { alarm.style.opacity=0.1; clearInterval(blink)}, 5000);
      playAlarm();
   }     

   this.onmousedown = function (evt) {
      if (input != null) {
         postEvent(obj.id, -1);
         endInput();                   
      }
   }

   function formatTime (hour, minute, second) {
      var t = {};
      t.seconds = second < 10?"0"+second.toString():second.toString();
      t.minutes = minute < 10?"0"+minute.toString():minute.toString();
      t.hours   = hour < 10?"0"+hour.toString():hour.toString();
      return t;
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
         hourHand.setAttribute("transform", "rotate(" + (hour+minute/60)*30 + " " + cx + " "  + cy + ")");
      } else if (digits.length > 0) {
          var t = formatTime(hour, minute, second);
          
          clearDigits();
          setDigit(0,t.seconds[0]);
          setDigit(1,t.seconds[1]);
          setDigit(2,t.minutes[0]);
          setDigit(3,t.minutes[1]);
          setDigit(4,t.hours[0]);
          setDigit(5,t.hours[1]);
      }
      if (pm != undefined) {
         pm.style.opacity = hour >= 12 ? 1.0 : 0.1;
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

   function beginInput (mode, init) {
      if (input != null)
         endInput();
      if (input == null) {         
         var ctm = label.getCTM();         
         input = createOverlay(root, inputArea, 'input');
         
         input.type = 'text';
         input.pattern = '([01]?[0-9]|2[0-3]):[0-5][0-9]';        
         input.style.fontFamily = config.labelFont.family;
         input.style.fontSize = config.labelFont.size*ctm.d + 'px';
         input.value = init;
         input.focus();
         
         label.style.display='none';
         inputArea.style.opacity = 1.0;
         inputArea.style.fill = '#E0E0F0';
                                   
         // Set up event handles on the input element.
         input.onmousedown = function (e) { e.stopPropagation(); }
         input.onkeydown = function (e) {
            var time = input.value.split(':');
            var sel = input.selectionStart;
            if (e.keyCode == 13 && input.checkValidity()) {
               var args;
               if (time.length >= 2)
                  args = [ mode, time[0], time[1], 0 ]; // set time or alarm
               else if (mode != 0)
                  args = [ mode, -1, -1 ]; // clear alarm   
               else
                  args = -1; // Nop   
               postEvent(obj.id, args);
               input.value = "";
               endInput();
            } else if (e.keyCode == 27) {
               postEvent(obj.id, -1);
               endInput();                   
            } else if (e.keyCode == 38) {
                if (time.length >= 2) {
                   if (sel <= time[0].length)
                      time[0] = parseInt(time[0])+1;
                   else   
                      time[1] = parseInt(time[1])+1;
                   if (time[0] == 24)
                      time[0] = 0;   
                   if (time[1] == 60)
                      time[1] = 0;   
                   if (time[1] < 10)
                     time[1] = '0'+time[1];                  
                   input.value = time[0]+':'+time[1];   
                   input.selectionStart = input.selectionEnd = sel;
                }
                e.preventDefault();
            } else if (e.keyCode == 40) {
                if (sel <= time[0].length)
                      time[0] = parseInt(time[0])-1;
                   else   
                      time[1] = parseInt(time[1])-1;
                   if (time[0] == -1)
                      time[0] = 23;   
                   if (time[1] == -1)
                      time[1] = 59;   
                   if (time[1] < 10)
                     time[1] = '0'+time[1];                  
                   input.value = time[0]+':'+time[1];   
                   input.selectionStart = input.selectionEnd = sel;
                e.preventDefault();
            }
         }         
      }
   }
   
   function endInput () {
      if (input != null) {
         deleteOverlay(input);         
         label.style.display='inline';
         inputArea.style.opacity = 0;
         input = null;
      }
   }
   

}
  
