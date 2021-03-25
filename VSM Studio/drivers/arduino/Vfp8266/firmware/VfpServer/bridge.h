/********************************************************************/
/********************************************************************/
/*****                                                          *****/
/*****        L A B C E N T E R    E L E C T R O N I C S        *****/
/*****                                                          *****/
/*****                  VFP Server for ESP8266                  *****/
/*****                                                          *****/
/*****                 Bridge Class - Top Level                 *****/
/*****                                                          *****/
/********************************************************************/
/********************************************************************/

#pragma once

#include <Arduino.h>
#include "RemoteDebug.h"

class PacketReader;
class CommandProcessor;

extern RemoteDebug remoteDebug;

#define debug remoteDebug

class BridgeServer 
 { public:
      BridgeServer (Stream &transport);
      bool begin ();
      void end();
      bool poll();
   private:
      Stream &transport;
      PacketReader *pr;
      CommandProcessor *cp;
      bool started;
 };  


