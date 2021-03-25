#pragma once

#include <core.h>
#include <Ethernet.h>
#include <EthernetClient.h>
#include <EthernetServer.h>
#include "NTPClient.h"

#define VFPSERVER 1

class RequestParser;
     
class VfpServer : public EthernetServer
 { public:
      VfpServer(uint8_t cs, uint16_t port);
      void begin (const char *mac, const char *ip);
      void setTitle (const String &title); 
      
      bool operator()(const String &object) { return requestObject.equalsIgnoreCase(object); }
      StreamList &parseArgs();      
      void set(const String &object, const String &property, int n) { set(object, property).arg(n).end(); }
      void set(const String &object, const String &property, long n) { set(object, property).arg(n).end(); }
      void set(const String &object, const String &property, float n) { set(object, property).arg(n).end(); }
      void set(const String &object, const String &property, const String &s) { set(object, property).arg(s).end(); }
      void set(const String &object, const String &property, const char *s) { set(object, property).arg(s).end(); }      
      PrintList &set (const String &object, const String &property);
      PrintList &call(const char *object, const __FlashStringHelper *method);
      PrintList &call(const String &object, const String &method);
      bool isRequestObject(const String &object) { return requestObject.equalsIgnoreCase(object); }      
      static void attachReloadHandler(void (*isr)()) { reloadHandler = isr; }
      static void attachRequestHandler(void (*isr)()) { requestHandler = isr; }
      static void attachRequestParser(const char *id, RequestParser *instance);
      static void attachEventHandler(const char *id, void (*isr)());
      void awaitRequest(uint32_t delay);
      
      void useTimeServer (const String &host, int8_t offset_hrs=0);
      
      void poll();


   private:
      void sendFile(const String &fileName, uint32_t timestamp, EthernetClient &client);
      void callEventHandlers(const char *id);
      uint32_t getTimeStamp (const String &fileName);
      uint8_t make83Name(const char* str, uint8_t* name);
      static time_t getNetworkTime();
      
      struct EventHandler
       {   const char *id;
           RequestParser *parser;
       	  void (*handler)();
       	  EventHandler *next;
       };
      
   private:
      String docTitle;
      bool firstPoll;
      bool reload, loading, newTitle;
      static void (*reloadHandler)();      
      static void (*requestHandler)();   
      static EventHandler *eventHandlers;
      String requestObject;
      EthernetClient requestStream;      
      StreamList requestList;
      EthernetClient statusStream;
      PrintBuffer statusBuffer;
      PrintList statusList;      
      NTPClient ntp;
      
 };

class RequestParser
 { public:
      virtual StreamList& parseArgs(StreamList &) = 0;
 };

extern VfpServer *VFP;
 

