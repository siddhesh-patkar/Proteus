 function LEDStrip(root, config) {
     var LED = root.getElementsByClassName('LED');
     var bounds = root.getElementsByClassName('bounds')[0];
     var text = root.getElementsByTagName("text")[0];
     var bbox = bounds.getBBox();
     var tbox;
     
     if (config.LEDGroupArea1 === undefined) {
        // Convert legacy config properties:
        config.LEDGroupArea1 = 100;
        config.LEDGroupArea2 = 0;
        config.LEDColourGroup1 = config.LEDonColour;
        config.LEDColourGroup2 = config.LEDonColour;
        config.LEDColourGroup3 = config.LEDonColour;
     }
      
     //setup
     this.reconfigure = function(newinstance) {
         if (newinstance) {
             config.textFont = getFontStyle(text.style)
             config.textVisible = "1"
         }
         
         // Configure the text label
         if (parseInt(config.textVisible)) {
            text.style.display = "inherit";
            setFontStyle(text.style, config.textFont)
            tbox = text.getBBox();
            text.style.textAnchor = 'middle'
            text.setAttribute('y', bbox.height + tbox.height);
            text.setAttribute('x', bbox.width / 2);
         } else {
            text.style.display = "none";
         }                      

         //allows user to set an initial value
         this.setValue(config.init)
     }


     this.setValue = function(location) {
         Area1 = parseInt(config.LEDGroupArea1)
         TotalArea = Area1

         Area2 = TotalArea + parseInt(config.LEDGroupArea2) > 100 ? 100 - TotalArea : parseInt(config.LEDGroupArea2);
         TotalArea += Area2
         config.LEDGroupArea2 = Area2

         Area3 = 100 - TotalArea
         TotalArea += Area3
         config.LEDGroupArea3 = Area3


         if (parseInt(config.units)) {
             if (location < 0) {
                 location = 0;
             } else if (location > 100) {
                 location = 100;
             }

             text.textContent = location + "%";
             this.setLEDs(location)

         } else {
             if (location < 0) {
                 location = 0;
             } else if (location > LED.length) {
                 location = LED.length;
             }

             text.textContent = (location / LED.length) * 100 + "%";
             this.setLEDs((location / LED.length) * 100)
         }


     }

     this.setLEDs = function(location) {

         var interval = ((1 / LED.length) * 100) / 2

         if (location < 5 && location >= 1) {
             location = 100 / LED.length;
         }

         for (var j = 0; j < LED.length; j++) {
             if (j < ((Area1-interval)/100)*LED.length) {
                 LED[j].style.fill = config.LEDColourGroup1;
             } else if (j < ((Area2+Area1-interval)/100)*LED.length) {
                 LED[j].style.fill = config.LEDColourGroup2;
             } else {
                 LED[j].style.fill = config.LEDColourGroup3;
             }
         }

         for (var j = LED.length - 1; j >= ((location - interval) / 100) * LED.length; j--) {
             LED[j].style.fill = config.LEDoffColour;
         }

     }

 }