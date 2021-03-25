/********************************************************************/
/********************************************************************/
/*****                                                          *****/
/*****        L A B C E N T E R    E L E C T R O N I C S        *****/
/*****                                                          *****/
/*****                 Visual Designer for Arduino              *****/
/*****                                                          *****/
/*****             Virtual Front Panel Server for ESP8266       *****/
/*****                                                          *****/
/********************************************************************/
/********************************************************************/

#include "Vfp8266.h"

using namespace Vfp8266;

Server VFP;

// Global list of event handlers:
void (*Server::reloadHandler)();
void (*Server::requestHandler)();         
Control *Server::controls = NULL;

Control::Control (const char *_id) : id(_id), reloadHandler(0), eventHandler(0) { VFP.addControl(this); }

Server::Server () 
 { // Request and status messages use Javascript compatible delimited lists:
   statusList.setDelimiter(',');
   requestList.setDelimiter(',');
   requestList.setTerminator('*');
 }

void Server::begin(int port) 
 { // Start the bridge:   
   Bridge.begin(115200);  

   // Initialize debug/console
   Console.begin();
   
   // Launch the server process:   
   Console.println(F("Starting Server Process..."));         
   process.begin(F("vfp"));  
   process.addParameter(String(port)); 
   process.runAsynchronously();    
   process.println(F("$.reset=1"));
 }

void Server::setTitle (const String &s)
 { set("$", F("title")).arg(s).end();
 }
 
PrintList &Server::call (const char *object, const __FlashStringHelper *method)
 { return call(object, String(method));
 }  
    
PrintList &Server::call (const String &object, const String &method)
// Control method call
 { process.print(object);
   process.print('.');
   process.print(method);
   process.print("(");
   statusList.setTerminator(')');
   return statusList.begin(true, &process);
 }
           
void Server::set (const char *object, const __FlashStringHelper *key,  const String &value)
 { set(object, String(key)).arg(value).end();
 }

PrintList &Server::set (const char *object, const __FlashStringHelper *key)
 { return set(object, String(key));
 }  
 
PrintList &Server::set(const String &object, const String &key)
// Control state assignment - values are re-assigned automatically on page reload.
 { process.print(object);
   process.print('.');
   process.print(key);
   process.print("=");
   statusList.setTerminator(0);
   return statusList.begin(true, &process);
 }

void Server::create (const char *object, const __FlashStringHelper *type)
// Create a control of the specified type on the JS side.
 { set("$.create", object).arg(type).end();
 }

void Server::record (const char *object, bool flag)
// Enable or disable method recording. This is used for controls that need to recover their history on a reload.
 { set("$", flag ? F("record") : F("stop")).arg(object).end();
 }

void Server::erase(const char *object)
// Erase recorded method calls for the specified control.
 { set("$", F("erase")).arg(object).end();
 }
 
 
void Server::addControl (Control *control)
// Add a control to the list managed by the server.
 { if (controls == NULL)
    { controls = control;
      control->next = NULL;
    }
   else
    { Control *tail = controls;
      while (tail->next != NULL)
         tail = tail->next;
      tail->next = control;
    }
 }       

void Server::waitForTimeServer ()  
 { while (timeStatus() != timeSet)
      awaitRequest(1000);   
 }
   
void Server::waitForRequests (uint32_t delayms)
 { uint32_t target = millis()+delayms;
   while ((long)(target-millis()) >= 0)
    { poll();
      delay(1);
    } 
 }
 
void Server::poll () 
 { while (process.available() > 0) 
    { String command = process.readStringUntil(':');
      String value;
      if (command == F("RELOAD"))
         callReloadHandlers();
      else if (command == F("EVENT"))
         callEventHandlers();
      else if (command == F("ADDRESS"))
         ipaddress.fromString(process.readStringUntil('*'));
      else if (command == F("TIME"))
       { time_t t = strtoul(process.readStringUntil('*').c_str(), NULL, 10);
         setTime(t);
       }
                            
      process.find('\n'); 
    }                      
 }

void Server::callReloadHandlers ()
 { Control *cp = controls;
   while (cp != NULL)   
    { cp->reload();
      cp = cp->next;
    }
   if (reloadHandler != NULL)
    { requestObject = (char *)0;
      reloadHandler();
    }
 }       
      
void Server::callEventHandlers ()
// Call the request parser and/or event handler(s) for one or more controls as passed in an EVENT message.
// The lines are formatted as id[.state]=[args]*
 { String state = process.readStringUntil('=');
   int dot = state.indexOf('.');
   if (dot != -1)
      requestObject = state.substring(0, dot);
   else
      requestObject = state;
  
   Control *cp = controls; 
   while (cp != NULL)
    { if (requestObject.equals(cp->id))
       { // Process the state assignment/request arguments:
         requestList.begin(&process);
         cp->parseArgs(requestList); 
         requestList.end();
        
         // Call the control's event handler, if present.
         if (cp->eventHandler != NULL)    
            cp->eventHandler();
      }
 	  cp = cp->next;		
 	}
  if (requestHandler != NULL)
 	  requestHandler();				
  requestObject = (char *)0;	  
 } 	 				
   


   