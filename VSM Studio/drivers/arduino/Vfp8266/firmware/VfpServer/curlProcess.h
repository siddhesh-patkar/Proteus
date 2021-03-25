/********************************************************************/
/********************************************************************/
/*****                                                          *****/
/*****        L A B C E N T E R    E L E C T R O N I C S        *****/
/*****                                                          *****/
/*****                  VFP Server for ESP8266                  *****/
/*****                                                          *****/
/*****                   Curl Command Process                   *****/
/*****                                                          *****/
/********************************************************************/
/********************************************************************/

#pragma once

#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>

#include "Process.h"

class CurlProcess : public Process
 { public:
      CurlProcess (int argc, String argv[]);
      ~CurlProcess();

      // Process Overloads
      virtual int availableOutput();
      virtual String readOutput(int maxlen);
      
   private:
      HTTPClient client;   
      String action, data;
 };
 
