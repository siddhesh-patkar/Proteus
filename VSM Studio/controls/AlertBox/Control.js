
require('alerts.js')
require('alerts.css')

function AlertBox (id) {    

     this.info = function (msg) { jInfo(msg, pageTitle, function() { postEvent(id); }); }
     this.warning = function (msg) { jWarning(msg, pageTitle, function() { postEvent(id); }); }
     this.error = function (msg) { jError(msg, pageTitle, function() { postEvent(id); }); }
     this.confirm = function (msg) { jConfirm(msg, pageTitle, function(flag) { if (flag) postEvent(id); }); }
     
     return this;
}   




