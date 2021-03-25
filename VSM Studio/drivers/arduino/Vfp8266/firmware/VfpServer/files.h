/********************************************************************/
/********************************************************************/
/*****                                                          *****/
/*****        L A B C E N T E R    E L E C T R O N I C S        *****/
/*****                                                          *****/
/*****                  VFP Server for ESP8266                  *****/
/*****                                                          *****/
/*****                     Bridge File System                    *****/
/*****                                                          *****/
/********************************************************************/
/********************************************************************/

#pragma once

#include <arduino.h>
#include <FS.H>

#include "bridge.h"
#include "command.h"

#define MAX_FILES 4

class Files : public Runner
 { public:
      Files (CommandProcessor *cp);
      ~Files ();
      
      // Command Handlers
      String OPEN_Command (const String &data);
      String CLOSE_Command (const String &data);
      String READ_Command (const String &data);
      String WRITE_Command (const String &data);
      String ISDIRECTORY_Command (const String &data);
      String SEEK_Command (const String &data);
      String TELL_Command (const String &data);
      String SIZE_Command (const String &data);
      
   private:
      int getId (const String &data) { return (unsigned char)(data[1]); }
      bool isFile (int id) { return id >= 0 && id < MAX_FILES && files[id]; }
      
   private:
      File files[MAX_FILES];
         
 };
 
