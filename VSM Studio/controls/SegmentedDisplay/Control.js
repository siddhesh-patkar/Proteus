function SegmentedDisplay (root, config)  {
   var elements = root.getElementsByClassName('element');
   var numDigits = elements.length;
   var digits = [];   
   var dp = 0;
   
   for (var i=0; i<numDigits; i++) {
      digits[i] = new NumericDigit(elements[i], config);
   }
 
      
   this.reconfigure = function (newInstance) {
      if (newInstance) {
         config.faceColour = elements[0].getElementsByClassName('digitFace')[0].style.fill;                  
         config.lampColour = elements[0].getElementsByClassName('segment')[0].style.fill;
      }

      // Reconfigure the digits:
      var x = 0, y = 0;
      var boundRects = root.getElementsByClassName("bounds");   
      for (var i=0; i<numDigits; ++i) {   
         // Colours etc.         
         digits[i].reconfigure(newInstance);
      }      
   }      

      
   this.setValue = function (val) {
      clear();
                        
      var firstDigit = numDigits-1;
      var lastDigit = 0;
      var p = val.length-1;

      for (var i=firstDigit; i>=lastDigit && p>=0; --i) {
         if (val[p] == '.') {
            setPoint(i, true);
            p--;
         }   
         if (p >= 0)   
            setDigit(i, val[p--])         
      }       
      
      if (p >= 0) {
         // If there are characters over, display an error state
         for (var i=0; i<numDigits; ++i) {
            setDigit(i, "E");
            setPoint(i, false);
         }   
      }
   }   
      
   function clear() {
      for (var i=0; i<numDigits; ++i) {
         setDigit(i, " ");
         setPoint(i, false);
      }        
   }
         
   function setDigit (d, v) {
      if (d >= 0 && d < numDigits)
         digits[d].setDigit(v);
   }


   function setPoint  (d, f) {
      if (d >= 0 && d < numDigits)
         digits[d].setPoint(f);
   }


}

function NumericDigit (root, config) {   
   var segs = root.getElementsByClassName("segment");
   var dp = root.getElementsByClassName("decpoint")[0];
   var numSegs = segs.length;
   var segState=0;
   var dpState=false;
   
   
   this.reconfigure = function () {
      var face = root.getElementsByClassName('digitFace')[0];
      face.style.fill = config.faceColour;
      this.setSegments(0);
      this.setPoint(0);
   }
   
   this.setDigit = function (v) {
      // The segtables could be extended to support other types of display, e.g. 16 segment, with the ability to display the full alphabet.
      const segTable = [
         /* 0-9  */ 0x3F, 0x06, 0x5B, 0x4F, 0x66, 0x6D, 0x7D, 0x07, 0x7F, 0x67,                   
         /* A-F  */ 0x77, 0x7C, 0x39, 0x5E, 0x79, 0x71,
         /* -,R  */ 0x40, 0x50
      ];
      
      if (v >= '0' && v <= '9')
         this.setSegments(segTable[v.charCodeAt(0)-48]);
      else if (v >= 'A' && v <= 'F')
         this.setSegments(segTable[v.charCodeAt(0)-55]);
      else if (v == '-')
         this.setSegments(segTable[16]);
      else if (v == 'R')
         this.setSegments(segTable[17]);
      else   
         this.setSegments(0); 
   }
   
   this.setSegments = function (v) {
      for (var i=0; i<numSegs; ++i) {
         segs[i].style.opacity = (v & (1<<i)) ? config.lampOpacity.on : config.lampOpacity.off;
         segs[i].style.fill = config.lampColour;
      }
      segState = v;
   }

   this.setPoint = function (v) {
       dp.style.opacity = v ? config.lampOpacity.on : config.lampOpacity.off;
       dp.style.fill = config.lampColour;
       dpState = v;
   }
   
}      
