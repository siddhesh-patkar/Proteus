// Wrapper for the ESP8266 Curl Process

#ifndef __CLIENT_H__
#define __CLIENT_H__

#pragma once

#include <Arduino.h>
#include <Bridge.h>

namespace Vfp8266 
 { class Client 
    { public:
         Client();
         bool begin();
         bool operator() () { return success; }
         bool get(const String &url) { return doRequest(F("GET"), url); }
         bool post(const String &url) { return doRequest(F("POST"), url); }
         String readln();
      private:
         bool doRequest(const String &action, const String &url);   
      private:
         Process *request;      
         bool success;
    };       
 }    

extern Vfp8266::Client CLIENT;


#endif