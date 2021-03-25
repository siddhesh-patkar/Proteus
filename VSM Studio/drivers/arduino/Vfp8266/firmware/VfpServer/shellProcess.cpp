/********************************************************************/
/********************************************************************/
/*****                                                          *****/
/*****        L A B C E N T E R    E L E C T R O N I C S        *****/
/*****                                                          *****/
/*****                  VFP Server for ESP8266                  *****/
/*****                                                          *****/
/*****                  Shell Command Process                   *****/
/*****                                                          *****/
/********************************************************************/
/********************************************************************/

#include <Arduino.h>
#include <Time.h>
#include <FS.h>

#include "Bridge.h"
#include "ShellProcess.h"

ShellProcess::ShellProcess (int argc, String argv[])
// These class implements shell commands sufficient to permit basic operation of the bridge based filestore.
// TBD support for directory enumeration which (on the Yun) uses ls | AWK.
 { String filename = argv[1];
   if (!filename.startsWith("/"))
       filename = "/"+filename;      
   if (argv[0] == "ls")
    { // FileSystemClass::exists
      returnCode = SPIFFS.exists(filename) ? 0 : 1;
    }
   else if (argv[0] == "rm")
    { // FileSystemClass::remove
      returnCode = SPIFFS.remove(filename) ? 0 : 1;  
    }
   else
    { println("Unsupported command:"+argv[0]);
      returnCode = -1;
    }
 }

ShellProcess::~ShellProcess () {}

