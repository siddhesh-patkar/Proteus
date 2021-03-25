function Indicator (root, config) {    
      var obj = this;
      var symbols = root.getElementsByTagName("g");      
      var bounds = root.getElementsByClassName("bounds")[0];
      var bbox = bounds.getBBox();
      var label = root.getElementsByTagName("text")[0];
      var colour = root.getElementsByClassName("colour")[0];           
      
      //This code will run every time the user changes a property.            
      this.reconfigure = function (newinstance) {
      	//This grabs the pre existing styles from the SVG and puts them into the XML code,
      	//the two styles which are taken from the SVG are the label font and the LED colour.
      	//This will only run when the user drags the control from the list.
         if (newinstance) {
            config.labelFont = getFontStyle(label);
            if (colour!=undefined){
            config.ledColour = colour.style.fill;
           }else{
           	config.ledColour = undefined;
           }
         }   
         
         //This allows the user to set the colour of the LED. It takes the value from the XML and
         //applies it to the colour layer of the SVG.
         
         if(colour!=undefined){
   			 colour.style.fill = config.ledColour;
				 //code to try fix the blur refresh issue
				 //By transforming the SVG in any way it forces the web browser to update and allows the colour to update as well
				 //however the image i have already has a transform on it and therefore needs to keep it to remain looking nice
				 colour.setAttribute('transform', colour.getAttribute("transform"));  
         }
				 
				 //
         label.firstChild.nodeValue = config.labelText;
         label.style.pointerEvents = "none";   
         setFontStyle(label, config.labelFont);
         
         //This case is all to do with the location of the label. It takes the location set by the XML
         //and uses the bounds of the Indicator to set the location of the label.
         var x = null, y = null;
         var h = label.getBBox().height * 0.6;
         var s = parseInt(config.labelSpacing);
         switch (parseInt(config.labelPosition)) {
            case 1 : label.style.textAnchor = "middle"; x = bbox.width/2; y = -s;  break;
            case 2 : label.style.textAnchor = "middle"; x = bbox.width/2; y = bbox.height+s + h; break;
            case 3 : label.style.textAnchor = "end"; x = -s; y = bbox.height/2 + h/2; break;
            case 4 : label.style.textAnchor = "start";  x = bbox.width+s; y = bbox.height/2 + h/2; break;            
         }   
         
         //This allows the user to hide the text
         if (x != null && y != null) { 
            label.setAttribute('x', x);
            label.setAttribute('y', y); 
            label.style.display = "inherit"; 
         } else {
            label.style.display = "none"; 
         }
         
         //This sets the primary state of the LED to be off.           
         this.setState(false);
      };
      
     //This changes the state from "On" to "Off" by hiding and showing the "On" "Off" layers.      
     this.setState = function (down) {     	
         if (down) {
            symbols[0].style.display = 'none';
            symbols[1].style.display = 'inline';
         } else {
            symbols[0].style.display = 'inline';
            symbols[1].style.display = 'none';
         }  
     }         
}   




