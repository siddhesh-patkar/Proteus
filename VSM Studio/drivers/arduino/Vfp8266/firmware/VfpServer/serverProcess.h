/********************************************************************/
/********************************************************************/
/*****                                                          *****/
/*****        L A B C E N T E R    E L E C T R O N I C S        *****/
/*****                                                          *****/
/*****                  VFP Server for ESP8266                  *****/
/*****                                                          *****/
/*****                    Web Server Process                    *****/
/*****                                                          *****/
/********************************************************************/
/********************************************************************/

#pragma once

#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <ESP8266WebServer.h>

#include "Process.h"
#include "StringMap.h"

class ServerProcess : public Process
 { public:
      ServerProcess (int argc, String argv[]);
      ~ServerProcess();

      void run();            
      bool isRunning () { return server != NULL; }      

   protected:
       void handleFileRead (String path);
       void handleReload ();
       void handleSession ();
       void handleStatus ();
       void handleEvents ();
       void handleReset ();
       void syncTime();
       void pollStatus();
       void closeStatus();
       bool saveState (const String &line);
       void recordHistory (const String &id, const String &line);
       void eraseHistory (const String &id);
       
       static String getContentType (const String &filename);
                      
   protected:     
      char hostname[16];
      ESP8266WebServer *server;
      IPAddress connected;
      WiFiClient statusConn;
      String lineBuffer;
      String statusBuffer;
      long nextKeepAlive;
      long nextTimeout;
      StringMap sessionState;
      time_t nextSync;      
 };
 
