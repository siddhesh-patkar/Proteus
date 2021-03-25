require('moment.js');

function WindRose (root, config) {
   var petalLayer = root.getElementsByClassName('petals')[0];
   var ringLayer = root.getElementsByClassName('rings')[0];
   var center = root.getElementsByClassName('center')[0];
   
   // N.B. these are 'live' collections and will update automatically.
   var petals = root.getElementsByClassName('petal');
   var rings = root.getElementsByClassName('ring');   
   var background = root.getElementsByClassName('background')[0];
   
   // This is the data set, currently excluding the 'calm' region.
   var timeRange = parseInt(config.timeRange.range);
   var timeUnits = ["minute", "hour", "day", "month"];
   var timeUnit = timeUnits[parseInt(config.timeRange.unit)];
   var rawdata = [];
   var data = []
   var calm = 1;
      
   this.reconfigure = function (newInstance) {      
      if (newInstance) { 
         createPetals();
         createRings();
         updateData();
         
         root.getElementsByClassName('fake')[0].style.display = 'none';
         setFontConfig();
      }
      
      setFonts();
      setColours();
   } 

   this.loadData = function () {
      requestFile(config.filename, function(text) {
         rawdata = text.split('\n');
         updateData();
      });
   }
   
   this.setTimeRange = function (unit, range) {
      timeRange = range;
      timeUnit = timeUnits[unit];
      updateData();
   }     
   
   function setFontConfig () {
      config.directionFont = getFontStyle(root.getElementsByClassName('direction')[0]);
      config.percentageFont = getFontStyle(root.getElementsByClassName('ringText')[0]);
      config.keyFont = getFontStyle(root.getElementsByClassName("keyText")[0]);
   }
   
   function setFonts () {
      var keyTexts = root.getElementsByClassName("keyText");
      var keyTextPercentages = root.getElementsByClassName("keyTextPercentage");
      for (var c=0; c<=7; c++) {
         setFontStyle(root.getElementsByClassName('direction')[c],config.directionFont);
      }
      for (var c=0; c<=3; c++) {
         setFontStyle(root.getElementsByClassName('ringText')[c],config.percentageFont);
      }
      for (var c=0; c<=5; c++) {
         setFontStyle(keyTexts[c],config.keyFont);
         setFontStyle(keyTextPercentages[c],config.keyFont)
      }

      keyTextPercentages[5].style.fill = config.calm.textColour;
      keyTextPercentages[4].style.fill = config.band1.textColour;
      keyTextPercentages[3].style.fill = config.band2.textColour;
      keyTextPercentages[2].style.fill = config.band3.textColour;
      keyTextPercentages[1].style.fill = config.band4.textColour;
      keyTextPercentages[0].style.fill = config.band5.textColour;      
   }
   
   function createPetals () {
      var petalTemplate = root.getElementsByClassName('templates')[0];
      for (var i=0; i<=79; i++) {
         var newPetal = petalTemplate.cloneNode(true);
         newPetal.setAttribute("class", "petal");
         newPetal.setAttribute("pointer-events", "none");
         newPetal.setAttribute("transform","scale(1,1) rotate(" + (22.5 * i) + ")");
         petalLayer.appendChild(newPetal);
      }
      petalTemplate.style.display = 'none';
   }
   
   function createRings () {
      var ringTemplate = root.getElementsByClassName('templates')[1];
      for (var i=0; i<=3; i++) {
          var newRing = ringTemplate.cloneNode(true);
          newRing.setAttribute("class", "ring");
          newRing.setAttribute("pointer-events", "none");
          newRing.setAttribute("transform","scale(" + (i/4) + ")");
          newRing.style.strokeWidth = (4/i)+"px";
          ringLayer.appendChild(newRing);
      }
      ringTemplate.style.display = 'none';
   }
   
   function setColours() {
      for (var i=0; i<=15; i++) {
         petals[15-i].style.fill = config.band5.bandColour;
         petals[31-i].style.fill = config.band4.bandColour;
         petals[47-i].style.fill = config.band3.bandColour;
         petals[63-i].style.fill = config.band2.bandColour;
         petals[79-i].style.fill = config.band1.bandColour;
      }
      var keyText = root.getElementsByClassName('keyText');
      var colourKey = root.getElementsByClassName('colourKey');
      
      background.style.fill = config.backgroundColour;
      
      center.style.fill = config.calm.bandColour
       
      colourKey[0].style.fill = config.band5.bandColour;
      colourKey[1].style.fill = config.band4.bandColour;
      colourKey[2].style.fill = config.band3.bandColour;
      colourKey[3].style.fill = config.band2.bandColour;
      colourKey[4].style.fill = config.band1.bandColour;
      colourKey[5].style.fill = config.calm.bandColour;
       
      keyText[0].textContent = config.band5.label;
      keyText[1].textContent = config.band4.label;
      keyText[2].textContent = config.band3.label;
      keyText[3].textContent = config.band2.label;
      keyText[4].textContent = config.band1.label;
      keyText[5].textContent = config.calm.label;
   }
   
   function updatePetals () {
      for (var i=0; i<16; ++i) {
         // Create an array of 5 values which are stacked, starting at calm and working outwards.
         var values = [0, 0, 0, 0, 0];
         if (data[i] != undefined) {
            values[0] = parseFloat(data[i][0]) + calm;         
            for (var j=1; j<=4; j++)  
               values[j] = parseFloat(data[i][j]) + values[j-1];         
         }
         
         // Now scale the petals so that they extend to the values as set above.
         for (var j=0; j<=4; j++) {
            var t = petals[j*16+i].transform.baseVal.getItem(0);
            var s = values[4-j]/getTotal();
            t.setScale(s, s);
         }
      }            
   }
 
  function updateData () {
     var now = moment();
     var tmax = moment(now);
     var tmin = moment(now).subtract(timeRange, timeUnit);
     calm = 0;
     data = [];
     for (var d=0; d<16; ++d)
        data[d] = [0, 0, 0, 0, 0];
     for (var i=0; i<rawdata.length; ++i) {
        var row = CSVtoArray (rawdata[i]);
        if (row.length != 0) {
           var t = moment(row[0])
           if (t >= tmin && t <= tmax) {
              calm += parseInt(row[1]);
              for (var d=0; d<16; ++d)              
                 for (var b=0; b<5; ++b)
                    data[d][b] += parseInt(row[2+d*5+b]);
           }            
        }      
     }  
     if (getTotal() == 0)
       calm = 1;     
     updatePetals();
     updateLabels();
     rescale(); 
   }
 
   
   function updateLabels () {
        var percentages = root.getElementsByClassName("keyTextPercentage");            
        for (var i=0; i<=4; i++) {
           var value = 0;
           for (var j=0; j<16; j++)  
              if (data[j] != undefined)
                 value += parseFloat(data[j][4-i]);
           percentages[i].textContent = ((value/getTotal())*100).toFixed(1) + "%";
        }        
        percentages[5].textContent = ((calm/getTotal())*100).toFixed(1) + "%";
   }
   
   function rescale () {       
       
       // Find the largest scale of any petal. 
       var largest = 0, scale;
       for (var i=0; i<=15; i++){
           scale = petals[i].transform.baseVal.getItem(0).matrix.a;
           if (scale > largest)
              largest = scale;
       }
        
       // Set the scaling for the calm region:
       center.setAttribute("transform","scale(" + calm/getTotal()/largest + ")");

       // Set the scaling for all petals:        
       petalLayer.setAttribute("transform","scale("+((1/largest))+")");

       // Update the key
       var labels = root.getElementsByClassName('ringText'); 
       labels[0].textContent = Math.round(100*largest)+ "%";
       labels[1].textContent = Math.round(100*largest*0.75) + "%";
       labels[2].textContent = Math.round(100*largest*0.50)+ "%";
       labels[3].textContent = Math.round(100*largest*0.25)+ "%";
   }
 
  function getTotal() {
      var total = calm;
      for (var i=0; i<=4; i++)  
         for (var j=0; j<16; j++)  
            if (data[j] != undefined)
               total += data[j][i];
      return total;
   }
 
  if (typeof(editor) != "object")
     this.loadData();   
      
}