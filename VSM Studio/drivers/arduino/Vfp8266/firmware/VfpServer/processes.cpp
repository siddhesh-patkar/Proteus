/********************************************************************/
/********************************************************************/
/*****                                                          *****/
/*****        L A B C E N T E R    E L E C T R O N I C S        *****/
/*****                                                          *****/
/*****                  VFP Server for ESP8266                  *****/
/*****                                                          *****/
/*****                      Bridge Processes                    *****/
/*****                                                          *****/
/********************************************************************/
/********************************************************************/

#include "bridge.h"
#include "command.h"
#include "processes.h"
#include "CurlProcess.h"
#include "ServerProcess.h"
#include "ShellProcess.h"

Processes::Processes (CommandProcessor *command_processor) 
 { command_processor->registerCommand('R', this, (CommandHandler)&Processes::RUN_Command);
   command_processor->registerCommand('r', this, (CommandHandler)&Processes::RUNNING_Command);
   command_processor->registerCommand('W', this, (CommandHandler)&Processes::WAIT_Command);
   command_processor->registerCommand('w', this, (CommandHandler)&Processes::CLEANUP_Command);
   command_processor->registerCommand('O', this, (CommandHandler)&Processes::READ_OUTPUT_Command);
   command_processor->registerCommand('o', this, (CommandHandler)&Processes::AVAILABLE_OUTPUT_Command);
   command_processor->registerCommand('I', this, (CommandHandler)&Processes::WRITE_INPUT_Command);           
   memset(processes, 0, sizeof(processes));
   numProcesses = 0;
 }

Processes::~Processes ()
 { reset();
 }

void Processes::reset()
 { for (int i=0; i<numProcesses; ++i)
      if (isProcess(i))
         delete processes[i]; 
   numProcesses = 0;
 }   

void Processes::run()
 { for (int i=0; i<numProcesses; ++i)
      if (isProcess(i))
         processes[i]->run();
 }         

String Processes::RUN_Command (const String &data)
 { String args = data.substring(1);
   String argv[MAX_ARGS];
   int argc;
   for (argc=0; argc<MAX_ARGS; ++argc)
    { int sep = args.indexOf('\xFE');
      if (sep != -1)
       { argv[argc] = args.substring(0, sep);
         args = args.substring(sep+1);
       }
      else
       { argv[argc++] = args;
         break;
       }
    }      
   int id = launch(argc, argv);
   String result;
   if (id == -1)
    { result += '\x01';
      result += '\x00';
    }
   else
    { result += char(0);
      result += char(id);
    }
   return result;
 }

String Processes::RUNNING_Command (const String &data)
 { int id = getId(data);
   bool running;
   if (isProcess(id) && processes[id] != NULL)
      running = processes[id]->isRunning();
   else
      running = false;
   return boolResult(running);
 }

String Processes::WAIT_Command (const String &data)
 { int id = getId(data);
   int result;
   if (isProcess(id) && processes[id] != NULL)
      result = processes[id]->wait();
   return wordResult(result);
 }

String Processes::CLEANUP_Command (const String &data)
 { int id = getId(data);
   if (isProcess(id) && processes[id] != NULL)
    { delete processes[id];
      processes[id] = NULL;     
    }
   return voidResult();
 }

String Processes::READ_OUTPUT_Command (const String &data)
 { int id = getId(data);
   int maxlen = data[2];
   String result;
   if (isProcess(id) && processes[id] != NULL)
      result = processes[id]->readOutput(maxlen);   
   return result;
 }

String Processes::AVAILABLE_OUTPUT_Command (const String &data)
 { int id = getId(data);
   int result;
   if (isProcess(id) && processes[id] != NULL)
      result = processes[id]->availableOutput();
   if (result > 255)   
      result = 255; 
   return byteResult(result);
 }

String Processes::WRITE_INPUT_Command (const String &data)
 { int id = getId(data);
   if (isProcess(id) && processes[id] != NULL)
      processes[id]->writeInput(data.substring(2));
   return voidResult();
 }

int Processes::launch (int argc, String argv[])
 { Process *process = NULL;
   int nextId;
   for (nextId = 0; nextId < MAX_PROCESSES; ++nextId)
      if (!isProcess(nextId))
         break;   
   
   if (nextId < MAX_PROCESSES)      
    { if (nextId >= numProcesses)
         numProcesses = nextId+1;
      if (argv[0] == "vfp")
         process = new ServerProcess(argc, argv);
      else if (argv[0] == "curl")
         process = new CurlProcess(argc, argv);         
      else 
         process = new ShellProcess(argc, argv);
    }

   String cmdLine;
   for (int i=0; i<argc; ++i)
      cmdLine += argv[i] + ' ';      
   
   if (process != NULL)
    { debug.println("RUN:"+cmdLine);
      processes[nextId] = process;      
    }
   else
    { debug.println( "FAILED: "+cmdLine);
    }
   
   return nextId;
 }   
