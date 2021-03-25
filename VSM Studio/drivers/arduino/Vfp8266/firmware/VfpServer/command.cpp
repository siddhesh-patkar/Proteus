/********************************************************************/
/********************************************************************/
/*****                                                          *****/
/*****        L A B C E N T E R    E L E C T R O N I C S        *****/
/*****                                                          *****/
/*****                  VFP Server for ESP8266                  *****/
/*****                                                          *****/
/*****             Bridge Top Level - Command Processor         *****/
/*****                                                          *****/
/********************************************************************/
/********************************************************************/

#include "bridge.h"
#include "packet.h"
#include "command.h"
#include "processes.h"
#include "files.h"
#include "console.h"

CommandProcessor::CommandProcessor (BridgeServer *bridge, Stream &stream)
                : transport(stream)
 { finished = false; 
   numRunners = numCommands = 0;
   registerRunner(new RESET_Handler(this));
   registerRunner(new Processes(this));
   registerRunner(new Files(this));
   registerRunner(new Console(this));
 }

CommandProcessor::~CommandProcessor ()
 { for (int i=0; i<numRunners; ++i)
      delete runners[i];      
   for (int i=0; i<numCommands; ++i)
      delete commands[i];      
 }

void CommandProcessor::registerRunner (Runner *runner)
 { if (numRunners < MAX_RUNNERS)
      runners[numRunners++] = runner; 
 }

void CommandProcessor::registerCommand (char key, Runner *runner, CommandHandler handler)
 { if (numCommands < MAX_COMMANDS)
      commands[numCommands++] =  new Command (key, runner, handler);
 }      

void CommandProcessor::run ()
 { for (int i=0; i<numRunners; ++i)
      runners[i]->run(); 
 }      

String CommandProcessor::process (const String &data)
 { if (data == "XXXXX")
    { finished = true;
      return String();
    }

   if (data.startsWith("XX"))
      for (int i=0; i<numRunners; ++i)
         runners[i]->reset(); 
   
   for (int i=0; i<numCommands; ++i)
      if (commands[i]->isCmd(data[0]))
          return commands[i]->run(data);
        
   return String(); 
 }

RESET_Handler::RESET_Handler (CommandProcessor *cp) : Runner ()
{  cp->registerCommand('X', this, (CommandHandler) &RESET_Handler::RESET_Command);
 }
 
String RESET_Handler::RESET_Command (const String &data)
// The reset command handler returns the bridge version number.
 { String result; 
   result += char(0); 
   result += "160"; 
   return result; 
 }
    
   

