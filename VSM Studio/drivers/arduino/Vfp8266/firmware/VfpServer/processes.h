/********************************************************************/
/********************************************************************/
/*****                                                          *****/
/*****        L A B C E N T E R    E L E C T R O N I C S        *****/
/*****                                                          *****/
/*****                Proteus VSM - Yun Shield / Bridge         *****/
/*****                                                          *****/
/*****                        Console Header                    *****/
/*****                                                          *****/
/********************************************************************/
/********************************************************************/

#pragma once

#include "bridge.h"
#include "process.h"

#define MAX_PROCESSES 32
#define MAX_ARGS 4

class Processes : public Runner
 { public:
      Processes (CommandProcessor *cp);
      ~Processes ();
      void reset();
      void run();

      // CommandHandlers
      String RUN_Command (const String &data);
      String RUNNING_Command (const String &data);
      String WAIT_Command (const String &data);
      String CLEANUP_Command (const String &data);
      String READ_OUTPUT_Command (const String &data);
      String AVAILABLE_OUTPUT_Command (const String &data);
      String WRITE_INPUT_Command (const String &data);

   private:
      int launch (int argc, String argv[]);
      int getId (const String &data) { return (unsigned char)(data[1]); }
      bool isProcess(int id) { return id >= 0 && id < numProcesses && processes[id] != NULL; }
      
   private:
      Process *processes[MAX_PROCESSES];         
      int numProcesses;
 };
 
 
