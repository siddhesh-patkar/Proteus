
function Photo (root, config) {
   var obj = this;
   var border = root.getElementsByTagName('rect')[0];
   var bounds = root.getElementsByTagName('rect')[1];
   var imageRegion = root.getElementsByTagName('g')[0];
   var noImageText = imageRegion.getElementsByTagName('text')[0];   
   
            
   var imageArea = imageRegion.getElementsByTagName('rect')[0];  
   var image = null;
   var video = null;
   var source = null;
      
   noImageText.style.pointerEvents='none';
   
   this.reconfigure = function (newInstance) {
      if (newInstance) {        
      }
            
   };
   
   this.update = function () {
      var ctm = root.getCTM();
      var tbt = panel.createSVGTransform();
      tbt.setScale(1/ctm.a, 1/ctm.d);
      border.transform.baseVal.initialize(tbt);
      border.width.baseVal.value = bounds.width.baseVal.value*ctm.a;
      border.height.baseVal.value = bounds.height.baseVal.value*ctm.d;            
   };
 
   this.setFile = function (filename) {
      if (filename.toLowerCase().substr(-4,4) == '.jpg')
         showImage(filename)
      else if (filename.toLowerCase().substr(-5,5) == '.jpeg')
         showImage(filename)
      else if (filename.toLowerCase().substr(-4,4) == '.png')
         showImage(filename)
      else if (filename.toLowerCase().substr(-4,4) == '.bmp')
         showImage(filename)
      else if (filename.toLowerCase().substr(-4,4) == '.mp4')
         showVideo(filename)
      else 
         clear();
   };
 
   this.clear = function (filename) {
      if (image != null) {
         deleteOverlay(image);
         image = null;
      }
      if (video != null) {
         deleteOverlay(video);
         video = null;
      }
   }
   
   function showImage (filename) {
      if (video != null) {
         deleteOverlay(video)
         video = null;
      }
      if (image == null)
         image = createOverlay(root, imageArea, 'img');
      image.style.display = 'inline';      
      image.style.pointerEvents = 'none';
      image.src = filename;
   };

   function showVideo (filename) {
      if (image != null) {
         deleteOverlay(image);
         image = null;
      }
      if (video == null) 
         video = createOverlay(root, imageArea, 'video');
      else   
         video.removeChild(source)   
      source = document.createElement('source');
      source.type = 'video/mp4'
      source.src = filename;
      video.appendChild(source);      
      if (true || config.controls!=0) {
         video.setAttribute('controls', '');
         // video.load();
      }
      else {
         video.style.pointerEvents = 'none';
         video.removeAttribute('controls');   
         //video.play();
      }
   };

   function clear() {
      if (image != null) {
         deleteOverlay(image);
         image = null;
      }  
      if (video != null) {
         deleteOverlay(video);
         video = null;
      }   
   } 
   
   return this;
}
   

