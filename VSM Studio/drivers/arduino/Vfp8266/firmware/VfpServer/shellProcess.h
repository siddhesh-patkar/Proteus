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

#pragma once

#include <ESP8266WiFi.h>

#include "Process.h"

class ShellProcess : public Process
 { public:
      ShellProcess (int argc, String argv[]);
      ~ShellProcess();
 };
 
