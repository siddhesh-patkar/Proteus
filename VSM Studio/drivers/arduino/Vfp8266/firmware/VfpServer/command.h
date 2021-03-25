/********************************************************************/
/********************************************************************/
/*****                                                          *****/
/*****        L A B C E N T E R    E L E C T R O N I C S        *****/
/*****                                                          *****/
/*****                  VFP Server for ESP8266                  *****/
/*****                                                          *****/
/*****                 Bridge / Command Processor               *****/
/*****                                                          *****/
/********************************************************************/
/********************************************************************/

#pragma once

#include <arduino.h>

#define MAX_RUNNERS  10
#define MAX_COMMANDS 26

class CommandProcessor;
class Runner;
 
class Runner 
 { public:
      Runner ()  {}
      virtual ~Runner() {}
      virtual void reset() {};
      virtual void run() {};      

   protected:      
      String voidResult () { return String(); }
      String boolResult (bool b) { String r; r += b ? char(1) : char(0); return r; }
      String charResult (char c) { String r; r += c; return r; }      
      String byteResult (uint8_t b) { String r; r += char(b); return r; }      
      String wordResult (uint16_t w) { String r; r += char(w >> 8); r += char(w & 0xFF); return r; }      

 };

typedef String (Runner::*CommandHandler)(const String &data);

class Command
 { public:
      Command() {}
      Command (char c, Runner *r, CommandHandler h) : cmd(c), runner(r), handler(h) {}
      void reset() { runner->reset(); }
      bool isCmd(char c) { return cmd == c; }
      String run (const String &data) { return (runner->*handler)(data); }
   private:   
      char cmd;
      Runner *runner;
      CommandHandler handler;
 };



class CommandProcessor 
 { public:
      CommandProcessor (BridgeServer *bridge, Stream &stream);
      ~CommandProcessor();
      
      void registerRunner (Runner *runner); 
      void registerCommand (char key, Runner *runner, CommandHandler handler);
      
      void run ();
      String process (const String &data);
   
      
   protected:           
      Stream &transport;
      Runner *runners[MAX_RUNNERS];
      Command *commands[MAX_COMMANDS];
      int numRunners, numCommands;
      bool finished;           
                  
 };

class RESET_Handler : public Runner 
 { public:
      RESET_Handler (CommandProcessor *cp);
      String RESET_Command (const String &data);
  
 };

