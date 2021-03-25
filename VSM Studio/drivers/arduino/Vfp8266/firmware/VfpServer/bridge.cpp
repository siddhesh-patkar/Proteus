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

#include "bridge.h"
#include "packet.h"
#include "command.h"

BridgeServer::BridgeServer (Stream &stream) : transport(stream)  
 { pr = new PacketReader(stream);
   cp = new CommandProcessor(this, stream);
   started = false;
 }

bool BridgeServer::begin ()
 { if (!started)
    { String line;
      unsigned long start = millis();
      do
       { if (transport.available())
          { char c = transport.read(); 
            if (c == '\n')
               break;
            else if (c != '\r')   
               line += c;
          }
       } while (millis()-start < 1000);

      started = line.startsWith("run-bridge");
    }     
   return started;   
 }

void BridgeServer::end()
 { started = false;
 }

bool BridgeServer::poll()
 { if (started)
      pr->process(cp);
   return started;      
 }

