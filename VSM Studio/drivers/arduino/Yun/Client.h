// Wrapper for the Yun FileIO library

#ifndef __CLIENT_H__
#define __CLIENT_H__

#pragma once

#include <HttpClient.h>

namespace Yun 
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

extern Yun::Client CLIENT;


#endif