function TextControlUtility (object, ctrlName) {
   this.obj = $(object);

   this.getNamedItem = function(obj, el, name) {
      var item = obj.find("" + el + "[name='" + name + "']").eq(0);
      if (item === undefined) {
         console.log("Text control invalid. Cannot find " + el + "[name='" + ctrlName + "']");
      }      
      return item;
   };

   this.ctrl = this.getNamedItem(this.obj, "rect", ctrlName);
   this.textSample = this.getNamedItem(this.obj, "text", "TextSample");
      
   var panelRect = $("#container").get(0).getBoundingClientRect();
   var ctrlRect = this.ctrl.get(0).getBoundingClientRect();
   this.clientRect = {
      top : ctrlRect.top - panelRect.top,
      left : ctrlRect.left - panelRect.left,
      width : ctrlRect.width,
      height : ctrlRect.height
   }
   
   var textNodeRect = this.textSample.get(0).getBoundingClientRect();
   this.paddingHorz = textNodeRect.left - ctrlRect.left;
   this.paddingVert = textNodeRect.top - ctrlRect.top;
   
   this.copyTextStyles = function(fromEl, toEl) {
      toEl.css("fontFamily", fromEl.css("fontFamily"));
      toEl.css("fontStyle", fromEl.css("fontStyle"));
      toEl.css("fontSize", fromEl.css("fontSize"));
      toEl.css("fontWeight", fromEl.css("fontWeight"));
      toEl.css("fontVariant", fromEl.css("fontVariant"));
      toEl.css("fontStretch", fromEl.css("fontStretch"));
      toEl.css("lineHeight", fromEl.css("lineHeight"));
      toEl.css("letterSpacing", fromEl.css("letterSpacing"));
      toEl.css("writingMode", fromEl.css("writingMode"));
      toEl.css("textAnchor", fromEl.css("textAnchor"));
      toEl.css("textAlign", fromEl.css("textAlign"));
      if (fromEl.css("fill") !== undefined)
         toEl.css("color", fromEl.css("fill"));
      else
         toEl.css("color", fromEl.css("color"));
   };
};

