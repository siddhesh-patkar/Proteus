/********************************************************************/
/********************************************************************/
/*****                                                          *****/
/*****        L A B C E N T E R    E L E C T R O N I C S        *****/
/*****                                                          *****/
/*****                  VFP Server for ESP8266                  *****/
/*****                                                          *****/
/*****                        Bridge Console                    *****/
/*****                                                          *****/
/********************************************************************/
/********************************************************************/

#include "bridge.h"
#include "console.h"

Console::Console (CommandProcessor *cp) : Runner()
 { cp->registerCommand('P', this, (CommandHandler)&Console::WRITE_Command); 
 }

Console::~Console ()
 { 
 }
 
void Console::run ()
 { 
 }
      
String Console::WRITE_Command (const String &data)
 { remoteDebug.print(data.substring(1));
   return voidResult();
 }

String Console::READ_Command (const String &data)
 { int len = data[1]; // max data to return.
   String result;
   return result;
 }

String Console::CONNECTED_Command (const String &data)
 { return boolResult(remoteDebug.isActive(remoteDebug.DEBUG));
 }
