
function ToggleSwitch (root, config) {    
      var obj = this;
      var orient = root.firstElementChild;
      var symbols = orient.getElementsByTagName("g");      
      var bounds = root.getElementsByClassName("bounds")[0];
      var bbox = bounds.getBBox();
      var label = root.getElementsByClassName("text")[0];  
      var label2 = root.getElementsByClassName("text2")[0];  
      var latched = false;
      var pressed = false;
      var labelPosition = null;
      var orientation = parseInt(config.orientation);
      
      
      setHotSpot(this, root);
      
      // We use style.display to avoid conflicts with inkscape   
      symbols[0].removeAttribute('display');
      symbols[1].removeAttribute('display');
      
      
      
      this.reconfigure = function (newinstance) {
         if (newinstance) {
            config.labelFont = getFontStyle(label); 
         }   
   			 
   			 label.firstChild.nodeValue = config.labelText1;
         label2.firstChild.nodeValue = config.labelText2;
         setFontStyle(label, config.labelFont);
         setFontStyle(label2, config.labelFont);
         
         switch (orientation) {
         case 0 : orient.setAttribute('transform', 'rotate(0)'); labelPosition = 1; break;
         case 1 : orient.setAttribute('transform', 'rotate(90,'+ bbox.width/2 +','+ bbox.height/2 +')');  labelPosition = 2; break;
      }
         

         
         var x = null, y = null;
         var h = label.getBBox().height * 0.6;
         var s = parseInt(config.labelSpacing);
         switch (parseInt(labelPosition)) {
            case 1 : label.style.textAnchor = "middle";  x = bbox.width/2; y = -s; label.setAttribute('transform', 'rotate(0,'+ bbox.width/2 +','+ bbox.height/2 +')'); 								   
                     label2.style.textAnchor = "middle"; x2 = bbox.width/2; y2 = bbox.height+s + h;	label2.setAttribute('transform', 'rotate(0,'+ bbox.width/2 +','+ bbox.height/2 +')'); break;
            case 2 : label.style.textAnchor = "start";   x = (bbox.width*1.5)+s; y = bbox.height/2 + h/2; label.setAttribute('transform', 'rotate(-90,'+ bbox.width/2 +','+ bbox.height/2 +')');   
                     label2.style.textAnchor = "end";    x2 = (bbox.width*(-0.5))-s; y2 = bbox.height/2 + h/2;	label2.setAttribute('transform', 'rotate(-90,'+ bbox.width/2 +','+ bbox.height/2 +')'); break;          
         }   
         
         if (x != null && y != null) { 
            label.setAttribute('x', x);
            label.setAttribute('y', y); 
            label.style.display = "inherit";
            label2.setAttribute('x', x2);
            label2.setAttribute('y', y2); 
            label2.style.display = "inherit"; 
         } else {
            label.style.display = "none"; 
         }
         

         
         symbols[0].removeAttribute('display');
         symbols[1].removeAttribute('display');
         
         this.setState(false, true);
      };
      
      this.onmousedown = function (evt) {
         latched = pressed;
         this.setState(!latched);                   
         buttonClick();
      };
      
      this.onmousemove = function(evt) {
         var p = getEventPos(evt, bounds);
         var r = bbox;
         var hit = p.x >= r.x && p.y >= r.y && p.x<=r.x+r.width && p.y<=r.y+r.height;

         this.setState(hit ? !latched : latched);                
      };
      
      this.onmouseup = function (evt) {
         postState(obj.id, "state", pressed ? 1 : 0);    
         latched = pressed;
      };

  
     this.setState = function (down, force) {
         if (down != pressed || force == true) {
            display(down);
            pressed = down;
         }
     } 

      
     function display (down) {
         if (down) {
            symbols[0].style.display = 'none';
            symbols[1].style.display = 'inline';
         } else {
            symbols[0].style.display = 'inline';
            symbols[1].style.display = 'none';
         }         
     }    
     
     return this;
}   




