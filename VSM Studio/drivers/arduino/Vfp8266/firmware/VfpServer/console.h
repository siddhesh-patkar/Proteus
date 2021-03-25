/********************************************************************/
/********************************************************************/
/*****                                                          *****/
/*****        L A B C E N T E R    E L E C T R O N I C S        *****/
/*****                                                          *****/
/*****                  VFP Server for ESP8266                  *****/
/*****                                                          *****/
/*****                      Console Header                      *****/
/*****                                                          *****/
/********************************************************************/
/********************************************************************/

#pragma once

#include "bridge.h"
#include "command.h"

class Console : public Runner
 { public:
      Console ( CommandProcessor *cp);
      ~Console ();
      
      // Runner
      void run ();
      
      // CommandHandlers
      String WRITE_Command (const String &data);
      String READ_Command (const String &data);
      String CONNECTED_Command (const String &data);   
            
 };
 
