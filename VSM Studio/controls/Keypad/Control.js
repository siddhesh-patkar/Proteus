
function KeyPad (root, config) {    
      var obj = this;      
      var bounds = root.getElementsByClassName("bounds")[0];
      var bbox = bounds.getBBox();
      var keys = root.getElementsByClassName("keypad_key");
      var labels = root.getElementsByTagName("text");
      var cur_key_obj;
      var label = root.getElementsByClassName("label")[0];
      
      for (i=0;i<keys.length;i++) {
        setHotSpot(this, keys[i]);
        //if (keys[i].id.indexOf("key_") == 0) {
        if (keys[i].hasAttribute("vfp:key_value")) {
          keys[i].key_value = keys[i].getAttribute("vfp:key_value");
        }
      }

      function findKeyObj(element) {
        var key_obj = element;
        // Find the parent element containing the key_value
        while (key_obj && !key_obj.key_value) {
          key_obj = key_obj.parentElement; 
        }
        return key_obj;        
      }
      
      
      this.reconfigure = function (newinstance) {
         if (newinstance) {
            config.labelFont = getFontStyle(label);
            //config.keypadFont = getFontStyle(labels[0]);
         }   
   
         for (i=0;i<labels.length;i++) {   
           //setFontStyle(labels[i], config.keypadFont);
           labels[i].style.pointerEvents = "none";
         }
         
         label.firstChild.nodeValue = config.labelText;
         label.style.pointerEvents = "none";   
         setFontStyle(label, config.labelFont);
         
         var x = null, y = null;
         var h = label.getBBox().height * 0.6;
         var s = parseInt(config.labelSpacing);
         switch (parseInt(config.labelPosition)) {
            case 1 : label.style.textAnchor = "middle"; x = bbox.width/2; y = -s;  break;//Above
            case 2 : label.style.textAnchor = "middle"; x = bbox.width/2; y = bbox.height+s + h; break;//Below
            case 3 : label.style.textAnchor = "end"; x = -s; y = bbox.height/2 + h/2; break;//left
            case 4 : label.style.textAnchor = "start";  x = bbox.width+s; y = bbox.height/2 + h/2; break;//right          
         }  
         //This allows the user to hide the text
         if (x != null && y != null) { 
            label.setAttribute('x', x);
            label.setAttribute('y', y); 
            label.style.display = "inherit"; 
         } else {
            label.style.display = "none"; 
         }         

      };
      
      this.onmousedown = function (evt) {
        var key_obj = findKeyObj(evt.target);
        if (key_obj) {
          //key_obj.setAttribute("transform", "translate(2,2)");
          cur_key_obj = key_obj;
          this.onmousemove(evt);
          buttonClick();
        }                            
      };
      
      this.onmousemove = function(evt) {
         if (cur_key_obj) {
           var p = getEventPos(evt, bounds);
           var r = cur_key_obj.getBBox();
           var hit = p.x >= r.x && p.y >= r.y && p.x<=r.x+r.width && p.y<=r.y+r.height;
           if (hit) {
             cur_key_obj.setAttribute("transform", "translate(2,2)"); 
           } else {
             cur_key_obj.removeAttribute("transform");
           }
         }
      };

      this.onmouseup = function (evt) {
        if (cur_key_obj) {
          var key_obj = findKeyObj(evt.target);
          if (key_obj == cur_key_obj) {
            key_value = key_obj.key_value; 
            postEvent(obj.id, key_value);
            console.log("mouse up on key " + key_value);
          }
          cur_key_obj.removeAttribute("transform");
          cur_key_obj = null;
        }
      }; 
     
     return this;
}   




