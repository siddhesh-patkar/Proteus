function JoyStick (root,config) {
   var stick = root.getElementsByClassName("stick")[0];
   var scale = root.getElementsByClassName("limits")[0];

   var bbox = scale.getBBox();
   var kbox = stick.getBBox();
   
   var result = [0,0];
   
   //This is the radius
   var r = bbox.height/2;
  //getValue
   var mouseDownPos = 0;
   var mouseDownValue = 0;
   
   setHotSpot(this, stick);
    
  this.reconfigure = function (newInstance) {
	  //Resets the stick to the middle
	  if(parseInt(config.returnToCenter)){
          stick.setAttribute("transform", "translate(" + 0 + " " + 0 + ")")
		  result = [0,0]
		  }
  }
	
  this.onmousedown = function (evt, element) {
	  //Collects the X,Y coordinates of the mouse click
      var p = getEventPos(evt, scale); 
      var values = [p.x,p.y]
	  //Sends the X,Y coordinates to the setValue function
	  this.setValue(values);
  }
	
   this.onmousemove = function(evt, element) { 
      //Collects the X,Y coordinates of the mouse click
	  var p = getEventPos(evt, scale); 
     var values = [p.x,p.y]
	  //Sends the X,Y coordinates to the setValue function
	  this.setValue(values);
   };

   this.onmouseup = function(e) {
	  //Resets the stick to the middle
	  if(parseInt(config.returnToCenter)){
          stick.setAttribute("transform", "translate(" + 0 + " " + 0 + ")")
		  result = [0,0]
        postState(root.id, "value", result);
		  }
   }; 
   
   this.setValue = function(p) {
	  //Checks to see is the user wants the stick to jump back to the middle
	   if(parseInt(config.roundLimit)){
	   //It draws the hypotenuse from the X,Y 
	   var h = Math.sqrt(p[0]*p[0] + p[1]*p[1])
	   
	   //This will check to see if the hypotenuse is longer than the radius
	   //If it is it will scale the movement back accordingly 
	   if(Math.abs(h) > r){
		   p[0] = p[0]*(r/h)
		   p[1] = p[1]*(r/h)
	   }
	   //Transforms the stick
	   stick.setAttribute("transform", "translate("+ (p[0]) +" " + (p[1]) + ")");
	   
	   } else{
	   //Using bbox to limit the movement
	   if(p[0]  > r){
		   p[0] = r } 
	   if(p[1]  > r){
		   p[1] = r}  
	   if(p[0]  < -r){
		   p[0] = -r} 
	   if(p[1]  < -r){
		   p[1] = -r} 
		   //Transforms the stick
       stick.setAttribute("transform", "translate("+ (p[0]) +" " + (p[1]) + ")");
	   }

	   //result
	   result = [Math.round((p[0]/bbox.width)*200), Math.round((p[1]/bbox.height)*-200)];
      postState(root.id, "value", result);
   }
	        
   this.getValueX = function() { 
     return result[0];
    }
    
   this.getValueY = function() { 
     return result[1];
	  
    }
 }
  