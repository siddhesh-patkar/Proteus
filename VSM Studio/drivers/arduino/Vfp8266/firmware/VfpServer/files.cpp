/********************************************************************/
/********************************************************************/
/*****                                                          *****/
/*****        L A B C E N T E R    E L E C T R O N I C S        *****/
/*****                                                          *****/
/*****                Proteus VSM - Yun Bridge Model            *****/
/*****                                                          *****/
/*****                        Bridge Files                    *****/
/*****                                                          *****/
/********************************************************************/
/********************************************************************/

#include "Bridge.h"
#include "Files.h"

Files::Files (CommandProcessor *cp) : Runner()
 { cp->registerCommand('F', this, (CommandHandler)&Files::OPEN_Command);
   cp->registerCommand('f', this, (CommandHandler)&Files::CLOSE_Command);
   cp->registerCommand('G', this, (CommandHandler)&Files::READ_Command);
   cp->registerCommand('g', this, (CommandHandler)&Files::WRITE_Command);
   cp->registerCommand('i', this, (CommandHandler)&Files::ISDIRECTORY_Command);
   cp->registerCommand('s', this, (CommandHandler)&Files::SEEK_Command);
   cp->registerCommand('S', this, (CommandHandler)&Files::TELL_Command);
   cp->registerCommand('t', this, (CommandHandler)&Files::SIZE_Command);   
 }

Files::~Files ()
 { for (int i=0; i<MAX_FILES; ++i)
      files[i].close();
 }
      
String Files::OPEN_Command (const String &data)
 { String filename = data.substring(2);   
   String mode = data.substring(1,2);
   if (!filename.startsWith("/"))
       filename = "/"+filename;   
   debug.println( "OPEN: " + filename +","+data+","+mode);
   for (int i=0; i<MAX_FILES; ++i)
      if (!isFile(i))
        { files[i] = SPIFFS.open(filename, mode.c_str());
          return wordResult(files[i] ? i : -1);
        }
   
   return wordResult(-1);

 }

String Files::CLOSE_Command (const String &data)
 { int id = getId(data);
   if (isFile(id))
    { debug.println(String("CLOSE: ")+files[id].name());
      files[id].close();
    }
   return byteResult(0);
 }

String Files::READ_Command (const String &data)
 { int id = getId(data);
   int maxlen = data[2], count;
   String result;
   if (isFile(id) && files[id].available() > 0)
    { uint8_t *buffer = (uint8_t *)malloc(maxlen);
      count = files[id].read(buffer,  maxlen);
      result += char(0);
      for (int i=0; i<count; ++i)
          result += char(buffer[i]);
      free(buffer);    
      debug.println("READ: "+String(count)+" bytes");
    }
   else
      result = byteResult(0xFF); // error
   return result;
 }

String Files::WRITE_Command (const String &data)
 { int id = getId(data);
   int count = data.length()-2;
   int result;
   if (isFile(id))
    { result = files[id].write((uint8_t *)data.c_str()+2, count) != count;
      debug.println("WRITE: "+String(count)+" bytes)");      
    }
   return byteResult(result);
 }

String Files::ISDIRECTORY_Command (const String &data)
 { return voidResult();
 }

String Files::SEEK_Command (const String &data)
 { return voidResult();
 }

String Files::TELL_Command (const String &data)
 { return voidResult();
 }
String Files::SIZE_Command (const String &data)
 { return voidResult();
 }
