/********************************************************************/
/********************************************************************/
/*****                                                          *****/
/*****        L A B C E N T E R    E L E C T R O N I C S        *****/
/*****                                                          *****/
/*****                 Visual Designer for Arduino              *****/
/*****                                                          *****/
/*****              Virtual Front Panel (Web Server)            *****/
/*****                                                          *****/
/********************************************************************/
/********************************************************************/

#include <spi.h>
#include <sd.h>
#include "VfpEthernet.h"

// Global object accessed by the controls:
VfpServer *VFP;

// Global list of event handlers:
void (*VfpServer::reloadHandler)();      
void (*VfpServer::requestHandler)();         
VfpServer::EventHandler *VfpServer::eventHandlers = NULL;
      

VfpServer::VfpServer(uint8_t cs, uint16_t port) 
         : EthernetServer(port)
 { // CS pin hard wired IO10 in Ethernet Class   
   firstPoll = reload = true;
   loading = false;
   newTitle = false;
   
   // Request and status messages use Javascript compatible delimited lists:
   requestList.setDelimiter(',');
   statusList.setDelimiter(',');
   
   // Assign the global pointer:
   VFP = this;
   
 }
 
void VfpServer::begin (const char *mac, const char *ip)
 { byte mac_addr[6], ip_addr[4];
   
   mac_addr[0] = strtol(mac+0, NULL, 16); 
   mac_addr[1] = strtol(mac+3, NULL, 16);
   mac_addr[2] = strtol(mac+6, NULL, 16);
   mac_addr[3] = strtol(mac+9, NULL, 16);
   mac_addr[4] = strtol(mac+12, NULL, 16);
   mac_addr[5] = strtol(mac+15, NULL, 16);

   if (ip != NULL && *ip != 0 || !Ethernet.begin(mac_addr))
    { // Fixed IP address
      char *pos = (char *)ip;
      ip_addr[0] = strtol(pos, &pos, 10); pos++;
      ip_addr[1] = strtol(pos, &pos, 10); pos++;
      ip_addr[2] = strtol(pos, &pos, 10); pos++;
      ip_addr[3] = strtol(pos, &pos, 10); pos++;
      Ethernet.begin(mac_addr, ip_addr);
    }
   
   EthernetServer::begin();
   
 }

void VfpServer::useTimeServer (const String &host, int8_t offset_hrs)
 { ntp.begin(host.c_str(), offset_hrs);
   Serial.print("Time Server:");
   Serial.println(ntp.getTimeServer());
   setSyncInterval(SECS_PER_DAY);
   setSyncProvider(&getNetworkTime);
 }
 
void VfpServer::setTitle (const String &s)
 { if (docTitle != s)
    { docTitle = s;
      newTitle = true;
    }
 }
  
StreamList &VfpServer::parseArgs ()
 { return requestList.begin(&requestStream);
 }

PrintList &VfpServer::set (const String &object, const String &method)
 { if (method.length() != 0)
      statusBuffer.print(object+'.'+method);      
   else   
      statusBuffer.print(object+".state");
   statusBuffer.print("=");
   statusList.setTerminator(0);
   return statusList.begin(true, &statusBuffer);
 }

PrintList &VfpServer::call(const char *object, const __FlashStringHelper *method)
// Memory optimized function call - method are names stored in ROM.
 { statusBuffer.print(object);
   statusBuffer.print('.');
   statusBuffer.print(method);
   statusBuffer.print("(");
   statusList.setTerminator(')');
   return statusList.begin(true, &statusBuffer);
 }
   
PrintList &VfpServer::call (const String &object, const String &method)
// Alternate function call - intended for direct invocation from user code.
 { statusBuffer.print(object);
   statusBuffer.print('.');
   statusBuffer.print(method);
   statusBuffer.print("(");
   statusList.setTerminator(')');
   return statusList.begin(true, &statusBuffer);
 }

void VfpServer::attachRequestParser (const char *id, RequestParser *instance)
// Attach a request parser object, i.e. a control which parses request bodies.
 { EventHandler *ehp = eventHandlers;
   while (ehp != NULL)
    { if (strcmp(id, ehp->id) == 0 && ehp->parser == NULL)
         break;
      ehp = ehp->next;		  
    }
   if (ehp == NULL)
    { ehp = new EventHandler;      
      ehp->id = id;      
 	   ehp->handler = NULL;
 	   ehp->next = eventHandlers;
 	   eventHandlers = ehp; 	
 	 }
 	ehp->parser = instance; 
 }

void VfpServer::attachEventHandler (const char *id, void (*isr)())
// Attach a user event handler for the specified control.
 { EventHandler *ehp = eventHandlers;
   while (ehp != NULL)
    { if (strcmp(id, ehp->id) == 0 && ehp->handler == NULL)
         break;
      ehp = ehp->next;		  
    }
   if (ehp == NULL)
    { ehp = new EventHandler;
 	   ehp->id = id;
 	   ehp->parser = NULL;
  	   ehp->next = eventHandlers;
 	   eventHandlers = ehp; 	
 	 }
 	ehp->handler = isr;
 }
 
void VfpServer::awaitRequest (uint32_t delayms)
// This function provides a way for the program to wait whist still polling for requests.
// We may be able to hide/automatie this by arranging for the LOOP contstruct to create hooks into the cpu.delay mechanism.
 { uint32_t target = millis()+delayms;
   while ((long)(target-millis()) >= 0)
    { poll();
      delay(1);
    } 
 }
 
void VfpServer::poll ()
// Check if a request is pending, serve files for GET requests and call the interrupt handler
// if we have a POST request.
 { if (firstPoll)
    { // Do this here because we can't guarantee that the terminal has initialzied the baud rate during setup.
      Serial.print("VFP Server is at ");
      Serial.println(Ethernet.localIP());
      firstPoll = false;
    }    

   // If we've been given a time server then try and set the time here:
   time_t t;
   if (timeStatus() == timeNotSet)
      if (ntp.getTimeServer() != 0 && ntp.getLocalTime(t))
         setTime(t);
   
   if (statusStream)
    { if (statusBuffer.length() > 0)
       { statusStream.print(statusBuffer);
         statusStream.stop();
         statusBuffer.clear();
       }
    }
   else if (statusBuffer.length() > 1024)
    { Serial.println("Discarding unsent status messages.");
      do
       { // Too many queued messages so prune the oldest ones.
         String &s = statusBuffer.buffer();
         int firstLine = s.indexOf('\n');
         s = s.substring(firstLine);
       } while(statusBuffer.length() > 512);
    }

   EthernetClient client = EthernetServer::available();            
   if (client) 
    { // an http request ends with a blank line
      boolean currentLineIsBlank = true;
      int lineCount = 0;
      String firstLine, currentLine;
      uint32_t etag;
       
      while (client.connected()) 
       { if (client.available()) 
          { char c = client.read();        
               if (c == '\n' && currentLineIsBlank) 
             { Serial.println(firstLine);               
               if (firstLine.startsWith("GET"))
                { uint8_t e = firstLine.indexOf(" ", 4);
                  String fileName = firstLine.substring(4, e);
                  fileName.toLowerCase();
                  if (fileName == "/")
                   { fileName = "/panel.htm";
                     reload = newTitle = true;
                     statusStream = EthernetClient();
                   }
                  else if (fileName == "/status")
                   { // Send the header
                     client.println("HTTP/1.1 200 OK");
                     client.println("Content-Type: text/plain");
                     client.println("Connection: close");  
                     client.println();                        
                     
                     statusStream = EthernetClient();
                     
                     if (reload && reloadHandler != NULL)
                      { Serial.println("Reload\n");
                        requestObject = "";
                        reloadHandler();
                        reload = false;
                      }
                     
                     if (newTitle || reload)
                      { if (docTitle.length() > 0)
                         { statusBuffer.print("$title = ");
                           statusList.setTerminator(0);
                           statusList.begin(true, &statusBuffer).arg(docTitle).end();
                           newTitle = false;   
                         }
                      }                                               
                                         
                     if (statusBuffer.length() > 0)
                      { // Send pending status message(s) immediately:
                        client.print(statusBuffer);
                        client.stop();
                        statusBuffer.clear();
                      }
                     else
                      { // No status info to send so hang on to the connection so that we can use it for asynchronous messages:
                        statusStream = client;
                      }
                     return; 
                   }
                                       
                  if (SD.exists((char *)fileName.c_str()))
                   { // The file exists. If we have a valid timestamp and e-tag then we can, potentially
                     // send a 'not-modified' response.                     
                     uint32_t timestamp = getTimeStamp(fileName);
                     if (timestamp != 0 && etag == timestamp)
                      { Serial.println("Not Modified");
                        client.println("HTTP/1.1 304 Not Modified");
                        client.println();
                      } 
                     else   
                        sendFile(fileName, timestamp, client);
                   }
                  else
                   { // The file was not found.
                     client.println("HTTP/1.1 404 Not Found");
                     client.println("Content-Type: text/html");
                     client.println("Connection: close");  
                     client.println();
                     client.println("<!DOCTYPE HTML>");
                     client.println("<HTML>");
                     client.println("<HEAD><TITLE> 404 NOT FOUND </TITLE></HEAD>");
                     client.println("<BODY><H1>");
                     client.print(fileName);
                     client.println(" - file not found");
                     client.println("</H1></BODY>");
                     client.println("</HTML>");                
                   }             
                  break;
                }
               else if (firstLine.startsWith("POST"))
                {    
                  client.println("HTTP/1.1 200 OK");
                  client.println("Content-Type: text/plain");
                  client.println("Connection: close");  
                  client.println();                                          
                                       
                  int8_t s = firstLine.indexOf("/");
                  int8_t e = firstLine.indexOf(" ", s);
                  requestObject = firstLine.substring(s+1, e); 
                  requestStream = client;
                  callEventHandlers(requestObject.c_str());
                  requestObject = "";
                                     
                  // If we want we can send a status message by return here. This will speed up the handling of cases
                  // where virtual control actions cause updates to other controls on the front panel.
                  // 
                  // client.print(statusBuffer);
                      
                  break;
                }                            
             }
           
            if (c == '\n') 
             { // Starting a new line
               // Serial.println(currentLine);
               if (lineCount++ == 0)
                  firstLine = currentLine, etag=0;
               else if (currentLine.startsWith("If-None-Match"))
                { etag = strtoul(currentLine.c_str()+14, NULL, 16);
                  Serial.print("ETAG: ");
                  Serial.println(etag, HEX);
                }
               currentLineIsBlank = true;
               currentLine = "";
             } 
            else if (c != '\r') 
             { // Add a character to the current line
               currentLine = currentLine+c;   
               currentLineIsBlank = false;
             }
          }
       }
       
      // give the web browser a tick to receive the data
      delay(1);
       
      // close the connection:
      client.stop();
    }
 }

void VfpServer::sendFile (const String &fileName, uint32_t timestamp, EthernetClient &client)
// Serve up a file from the SD card to the client.
 { const char *contentType = NULL;
    if (fileName.endsWith("htm"))
       contentType = "text/html";
    else if (fileName.endsWith("js"))
       contentType = "application/javascript";
    else if (fileName.endsWith("svg"))
       contentType = "image/svg+xml";
    else if (fileName.endsWith("png"))
       contentType = "image/png";
    else if (fileName.endsWith("gif"))
       contentType = "image/gif";
    else if (fileName.endsWith("jpg"))
       contentType = "image/jpeg";
    else
       contentType = "application/octet-stream";

   // Start of response:
   client.println("HTTP/1.1 200 OK");
   client.print  ("Content-Type: ");
   client.println(contentType);
   client.println("Cache-Control: no-cache");
   if (timestamp != 0)     
    { client.print("ETag: ");
      client.println(timestamp, HEX);
    } 
   client.println("Connection: close");  // the connection will be closed after completion of the response
   client.println();
                   
   Serial.print("Sending '");
   Serial.print(fileName);
   Serial.print("' ");
   Serial.flush();
  
   File file = SD.open(fileName.c_str(), FILE_READ); 
   uint8_t buffer[2048];
   uint32_t start = millis();
   while (file.available())
    { uint16_t n;
      n = file.readBytes((char *)buffer, sizeof(buffer));
      client.write(buffer, n);
    }
   
   uint32_t elapsed = millis()-start;
   Serial.print(file.size()*1000L/elapsed);
   Serial.println(" Bytes/Sec");  
   file.close(); 
   
 }

void VfpServer::callEventHandlers (const char *id)
// Call the request parser and/or event handler(s) for control object id, and also the global request handler, if present.
 { EventHandler *ehp = eventHandlers;      	
   while (ehp != NULL)
    { if (strcmp(ehp->id, id) == 0)
       { if (ehp->parser != NULL)
         { requestList.begin(&requestStream);
           ehp->parser->parseArgs(requestList); 
           requestList.end();
         }
        if (ehp->handler != NULL)    
            ehp->handler();
      }
 	  ehp = ehp->next;		
 	}
  if (requestHandler != NULL)
 	  requestHandler();				
 } 	 				


uint32_t VfpServer::getTimeStamp (const String &fileName)
// Return the dos date/time stamp for of a file in the root folder.
// This is rather hard work and could be circumvented if we had access to the _file member of class File, or if
// we upgraded to a newer/improved SD library such as SdFat.
 { uint32_t timestamp = 0;
   File dir = SD.open("/");
   uint8_t name83[11];
   dir_t entry;

   if (make83Name(fileName.c_str(), name83) && dir)
    { dir.seek(0);            
      while (dir.read(&entry, sizeof(entry)) == sizeof(entry))
       { // done if past last used entry
         if (entry.name[0] == DIR_NAME_FREE) 
            break;

         // skip deleted entry and entries for 
         if (entry.name[0] == DIR_NAME_DELETED)
            continue;
      
         // only list files
         if (DIR_IS_FILE(&entry) && memcmp(entry.name, name83, 11) == 0)
          { timestamp = (uint32_t)entry.lastWriteDate << 16L | entry.lastWriteTime;
            break;
          }
       }         
    }
  
   dir.close();
  
   return timestamp;  
 }       

uint8_t VfpServer::make83Name(const char* str, uint8_t* name)      
// Convert a filename string to 8.3 FAT format (11 bytes)
// Returns true if successful.
 { uint8_t c;
   uint8_t n = 7;  // max index for part before dot
   uint8_t i = 0;

   // blank fill name and extension
   while (i < 11) 
      name[i++] = ' ';
   i = 0;

   // Skip leading '/'
   if (str[0] == '/')
      str++;   
   while ((c = *str++) != '\0') 
    { if (c == '.') 
       { if (n == 10) 
          { Serial.println("Two dots");
            return false;  // only one dot allowed
          }
         n = 10;  // max index for full 8.3 name
         i = 8;   // place for extension
       } 
      else 
       { // illegal FAT characters
         PGM_P p = PSTR("|<>^+=?/[];,*\"\\");
         uint8_t b;
         while ((b = pgm_read_byte(p++))) if (b == c) return false;
         // check size and only allow ASCII printable characters
         if (i > n || c < 0X21 || c > 0X7E)
          { Serial.println("illegal character");
            return false;
          }
         // only upper case allowed in 8.3 names - convert lower to upper
         name[i++] = c < 'a' || c > 'z' ?  c : c + ('A' - 'a');
       }
    }
 
   // must have a file name, extension is optional
   return name[0] != ' ';
 }

time_t VfpServer::getNetworkTime ()
 { time_t epoch;
   return VFP->ntp.getLocalTime(epoch) ? epoch : 0;
 }	