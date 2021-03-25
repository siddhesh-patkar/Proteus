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

#include "Bridge.h"
#include "CurlProcess.h"

CurlProcess::CurlProcess (int argc, String argv[])
 { action = "GET";
   for (int i=1; i<argc; ++i)
      if (argv[i] == "--request")
         action = argv[++i];
      else if (argv[i] == "--data")
         data = argv[++i];
      else if (argv[i].startsWith("-"))
         continue; 
      else
         client.begin(argv[i]);

   // The returnCode is the http result code, or 0 if we couldn't connect to the server.
   int result = -1;
   if (action == "GET")
      result = client.GET();
   else if (action == "POST")
      result = client.POST(data);
   else if (action == "PUT")
      result = client.PUT(data);         
   debug.print("Result code:");
   debug.println(result);      
   returnCode = (result > 0) ? 0 : 1;   
 }

CurlProcess::~CurlProcess ()
 { client.end();
 }
 
int CurlProcess::availableOutput()
// Return true if response data is available.
 { WiFiClient *stream = client.getStreamPtr();
   return stream ? stream->available() : 0;  
 }
      
String CurlProcess::readOutput (int maxlen)
// Read up to maxlen chars from the response.
 { WiFiClient *stream = client.getStreamPtr();
   String result;
   if (stream)
    { while (stream->available() && maxlen--)
         result += char(stream->read());
    }
   return result;        
 }


