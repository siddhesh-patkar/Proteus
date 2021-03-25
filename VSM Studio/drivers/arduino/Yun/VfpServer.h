#pragma once

#include <Core.h>
#include <IPAddress.h>
#include <Console.h>
#include <Process.h>

#define VFPSERVER 1

namespace Yun 
 { class Control 
    { friend class Server;
      public:
         Control (const char *_id);
         virtual void reload() { }
         virtual StreamList &parseArgs(StreamList &argList) { return argList; }
         void attachReloadHandler(void (*isr)()) { reloadHandler=isr;  }
         void attachEventHandler(void (*isr)()) { eventHandler=isr; }
         
      protected:
    	  	const char *id;
    	   void (*reloadHandler)();
         void (*eventHandler)();
      	Control *next; 	  	      
    };

   class Server 
    { public:
         Server();
         void begin (int port=8080);
         void setTitle (const String &title); 
         bool operator()(const String &object) { return requestObject.equalsIgnoreCase(object); }
         StreamList &parseArgs();      
         PrintList &call(const char *object, const __FlashStringHelper *method);
         PrintList &call(const String &object, const String &method);
         void set(const char *object, const __FlashStringHelper *key, const String &value);
         PrintList &set(const char *object, const __FlashStringHelper *key);
         PrintList &set(const String &object, const String &key);
         void create(const char *object, const __FlashStringHelper *key);
         void record(const char *object, bool flag=true);
         void erase(const char *object);
         bool isRequestObject(const char *object) { return requestObject.equalsIgnoreCase(object); }      
         
         void awaitRequest(uint32_t delay) { waitForRequests(delay); }
         void waitForRequests(uint32_t delay);          
         void waitForTimeServer();
 	      PrintList &debug() { return debugList.begin(true, &Console); }      
         void poll();
   
         static void addControl(Control *instance);
         static void attachReloadHandler(void (*isr)()) { reloadHandler = isr; }
         static void attachRequestHandler(void (*isr)()) { requestHandler = isr; }
   
      private:
         void callReloadHandlers();
         void callEventHandlers();
         
         struct ControlInfo
          { Control *control;
          	ControlInfo *next;
          };
         
      private:
         IPAddress ipaddress;
         static void (*reloadHandler)();      
         static void (*requestHandler)();   
         static Control *controls;
         Process process;
         String requestObject;
         StreamList requestList;
         PrintList statusList;      
 	      PrintList debugList;         
    };

   typedef Server VfpServer; // legacy
    
      
 };

typedef Yun::Control Control;
extern  Yun::Server VFP;

// Legacy 
#define SERVER VFP
 

