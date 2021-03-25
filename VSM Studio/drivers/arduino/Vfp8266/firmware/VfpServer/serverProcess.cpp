/********************************************************************/
/********************************************************************/
/*****                                                          *****/
/*****        L A B C E N T E R    E L E C T R O N I C S        *****/
/*****                                                          *****/
/*****                  VFP Server for ESP8266                  *****/
/*****                                                          *****/
/*****                    Web Server Process                    *****/
/*****                                                          *****/
/********************************************************************/
/********************************************************************/

#include <Arduino.h>
#include <Time.h>
#include <FS.h>

#include "Bridge.h"
#include "ServerProcess.h"

#define KEEPALIVE_INTERVAL 10000
#define KEEPALIVE_TIMEOUT  30000
#define SESSION_FILE "/session.dat"

#define debug remoteDebug

ServerProcess::ServerProcess (int argc, String argv[])
 { int port = argc == 2 ? argv[1].toInt() : 80;
   server = new ESP8266WebServer(port);
  
   server->on("/", HTTP_GET, [this](){ handleReload(); });
   server->on("/session", HTTP_GET, [this](){ handleSession(); });
   server->on("/status", HTTP_GET, [this](){ handleStatus(); });
   server->on("/event", HTTP_POST, [this](){ handleEvents(); });
   server->on("/reset", HTTP_GET, [this](){ handleReset(); });
      
   //called when the url is not defined here
   //use it to load content from SPIFFS
   server->onNotFound([=](){ handleFileRead(server->uri()); });

   // Start the web server:
   server->begin();
   
   sprintf(hostname, "ESP8266_%06X", ESP.getChipId());
   nextKeepAlive = 0;
   nextTimeout = 0;

   // Seconds to start of 2017:
   nextSync = 1483228800;

   // Announce the service:
   MDNS.addService("vfpserver", "tcp", port);
 }   

ServerProcess::~ServerProcess()
 { server->stop();
   delete server;
 }

void ServerProcess::run ()
 { server->handleClient();
   pollStatus();
   syncTime();
 }

void ServerProcess::handleReload () {
    IPAddress remoteIP = server->client().remoteIP();
    if  (connected == INADDR_NONE || connected == remoteIP) {
       debug.print("Client Connected: ");
       debug.println(remoteIP);
       println("RELOAD:*");
       handleFileRead("/panel.htm");  
       statusConn = WiFiClient();
       statusBuffer = String();
    } else {
       String ip = String(connected[0]) + '.' + String(connected[1]) + '.' + String(connected[2]) + '.' + String(connected[3]);
       server->send(401, "text/plain", "Appliance in use by another client: "+ip);
    }
}

void ServerProcess::handleFileRead (String path){
  if(path.endsWith("/")) path += "index.htm";  
  String contentType = getContentType(path);
  String pathWithGz = path + ".gz";
  if(SPIFFS.exists(pathWithGz) || SPIFFS.exists(path)){
    if(SPIFFS.exists(pathWithGz)) {
       path += ".gz";
    }
    debug.println("sending: " + path);
    File file = SPIFFS.open(path, "r");
    size_t sent = server->streamFile(file, contentType);
    file.close();
  } else {
      server->send(404, "text/plain", "File Not Found: " + path);      
  }
}

void ServerProcess::handleSession() {
   StringMapIterator iter;
   String response;
   
   debug.println("Sending Session State.");              
   
   for (iter=sessionState; iter != NULL; ++iter) {
      if (!iter.key().startsWith("$.record")) {
         response += iter.key()+'='+iter.value()+'\n';
      }
   }

   File file = SPIFFS.open(SESSION_FILE, "r");
   server->setContentLength(response.length()+file.size());
   server->send(200, "text/plain", response);
   server->client().write(file, HTTP_DOWNLOAD_UNIT_SIZE);
   file.close();
}

void ServerProcess::handleStatus() {
    IPAddress remoteIP = server->client().remoteIP();
    if  (connected == INADDR_NONE || connected == remoteIP) 
       connected = remoteIP;
    else {
       server->send(401, "text/plain", "Appliance in use.");
       return;
    }

   if (statusBuffer.length() > 0) {
      // Send buffered status data:
      debug.println("Sending buffered status:");
      debug.print(statusBuffer);
      debug.println("*");
      server->send(200, "text/plain", statusBuffer);
      statusBuffer = String();
   } else {
      // Capture the status connection by sending only a response header:
      // debug.println("Captured Status socket");
      String response;
      response += "HTTP/1.1 200 OK\n";
      response += "Content-Type: text/plain\n";
      response += "Connection: close\n\n";
      server->sendContent(response);
      statusConn = server->client();
   }   
   nextKeepAlive = millis() + KEEPALIVE_INTERVAL;
   nextTimeout = millis() + KEEPALIVE_TIMEOUT;
}

void ServerProcess::handleEvents() {
   if (server->hasArg("plain")) {
      String body = server->arg("plain");
      int ob = body.indexOf("{\n");
      int cb = body.lastIndexOf("}\n");
      body = body.substring(ob+2, cb);
      
      debug.println("handleEvents:");
      debug.println(body+"*");      

      while (body.length() > 0)
       { int nl = body.indexOf('\n');
         String line = body.substring(0, nl);
         if (line.length() > 0)
          { println("EVENT:"+line+"*");
            saveState(line);
          }
         body = body.substring(nl+1);
       }
               
   } else  
      debug.println("handleEvents: no body");
   
   server->send(200, "text/plain", "ok\n");
}  

void ServerProcess::handleReset () {
  server->send(200, "text/plain", "WiFi Reset - please connect to SSID "+String(hostname)+" on IP 192.168.4.1 to reconfigure."); 
  delay(2000);
  WiFi.disconnect(true); 
  ESP.restart(); 
  delay(2000);
}

void ServerProcess::syncTime () {
// Update the the time passed to the Arduino side. 
// With the ESP8266 core 2.3.0 the time() function returns a value that is offset by the timezone as passed to configTime() so represents
// seconds since the epoch in the local timezone. This is directly compatible with the Arduino side; no translation to/from UTC is required.
  time_t now = time(NULL);
  if (now > nextSync) {
      println("TIME:"+String(now)+"*");    
      nextSync = now+3600;
  }
}  

void ServerProcess::pollStatus () {
   String line;
   if (canReadLine())
    { do   
       { char c = read();
         if (c == '\n')
            break;
         else if (c != '\r')
            line = line+c;             
       } while (available());
    }

   if (line.length() > 0) {
      debug.println("Status:"+line);
      if (!saveState(line))
          line = String();
   } else if (millis() > nextKeepAlive) {
      line = "$.keepalive";
      nextKeepAlive += KEEPALIVE_INTERVAL;  
   }
      
   if (connected) {
       if (millis() > nextTimeout) {
          debug.println("Client Disconnected.");
          statusBuffer = String();
          connected = INADDR_NONE;
       } else if (line.length() > 0) {
   
          if (statusConn.connected()) {
             statusConn.print(line);    
             closeStatus();
          } else {
             // Status message is buffered.
             statusBuffer += line + '\n';   
          }                
       }
    }
}

void ServerProcess::closeStatus () {
   statusConn.stop();
   statusConn = WiFiClient();
   nextTimeout = millis()+KEEPALIVE_TIMEOUT;  
}

bool ServerProcess::saveState (const String &line) {
   bool dirty = false;
   int eq = line.indexOf('=');
   int dot = line.indexOf('.');   
   if (eq != -1) {
      String key = line.substring(0, eq);
      String value = line.substring(eq+1);
      if (key == "$.reset")
         SPIFFS.remove(SESSION_FILE);  
      else if (key == "$.create")
         dirty = true; // pass through to JS (creates control)
      else if (key == "$.record") {
         value.replace("\"", "");
         sessionState["$.record."+value] = "1";
      }
      else if (key == "$.stop") {
         value.replace("\"", "");
         sessionState["$.record."+value] = "0";
      }
      else if (key == "$.erase") {
         value.replace("\"", "");
         eraseHistory(value);
      }
      else if (!sessionState.contains(key) || sessionState[key] != value) {
         sessionState[key] = value;
         dirty = true;   
      }           
   } else {
      String id = line.substring(0, dot);
      if (sessionState["$.record."+id] == "1") {
         recordHistory(id, line);
      }
      dirty = true;   
   }
   return dirty;   
}

void ServerProcess::recordHistory (const String &id, const String &line) {
   File file = SPIFFS.open(SESSION_FILE, "a");
   file.println(line);
   file.close();    
}

void ServerProcess::eraseHistory (const String &id) {
   File src = SPIFFS.open(SESSION_FILE, "r");
   File dst = SPIFFS.open("/temp.dat", "w");
   
   while (src.available()) {
      String line = src.readStringUntil('\n');
      if (!line.startsWith(id+'.'))
         dst.println(line);
   }
   src.close();
   dst.close();
    
   SPIFFS.remove(SESSION_FILE);
   SPIFFS.rename("/temp.dat", SESSION_FILE);      
}

String ServerProcess::getContentType (const String &filename) {
  if(filename.endsWith(".htm")) return "text/html";
  else if(filename.endsWith(".html")) return "text/html";
  else if(filename.endsWith(".css")) return "text/css";
  else if(filename.endsWith(".js")) return "application/javascript";
  else if(filename.endsWith(".png")) return "image/png";
  else if(filename.endsWith(".gif")) return "image/gif";
  else if(filename.endsWith(".jpg")) return "image/jpeg";
  else if(filename.endsWith(".ico")) return "image/x-icon";
  else if(filename.endsWith(".svg")) return "image/svg+xml";
  else if(filename.endsWith(".xml")) return "text/xml";
  else if(filename.endsWith(".pdf")) return "application/x-pdf";
  else if(filename.endsWith(".zip")) return "application/x-zip";
  else if(filename.endsWith(".gz")) return "application/x-gzip";
  return "text/plain";
}

 
                       

