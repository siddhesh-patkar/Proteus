/********************************************************************/
/********************************************************************/
/*****                                                          *****/
/*****        L A B C E N T E R    E L E C T R O N I C S        *****/
/*****                                                          *****/
/*****                  IoT Builder for Proteus 8               *****/
/*****                                                          *****/
/*****                     ESP8266 VFP SERVER                   *****/
/*****                                                          *****/
/********************************************************************/
/********************************************************************/

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <ESP8266AVRISP.h>
#include <Ticker.h>
#include <Time.h>
#include <Updater.h>
#include <FS.h>

#include "WiFiManager.h"
#include "StringMap.h"
#include "RemoteDebug.h"
#include "FtpServer.h"
#include "Bridge.h"

// Globals:
BridgeServer bridge (Serial);
FtpServer ftpServer;   
RemoteDebug remoteDebug;
StringMap config;
Ticker ticker;

// Settings for the ICSP interface:
#define AVR_PORT 328
#define AVR_RESET 0 // We-Mos D3

// GPIO Pin for WIFI reset button (Playkit)
#define WIFI_RESET 4

ESP8266AVRISP avrprog(AVR_PORT, AVR_RESET);

void setup(void) {
   char host[16];
   sprintf(host, "ESP8266_%06X", ESP.getChipId());

   Serial.begin(115200);
   Serial.setDebugOutput(true);
   Serial.println("");
   
   // Open the filesystem:
   SPIFFS.begin();      
        
   pinMode(BUILTIN_LED, OUTPUT);
   pinMode(WIFI_RESET, INPUT_PULLUP);
   
   // Set defaults:
   config["appliance"] = "Virtual Front Panel (ESP8266)";
   config["timeserver"] = "pool.ntp.org";
   config["timeoffset"] = "+0";
   
   // Read configuration:
   File file = SPIFFS.open("/config.dat", "r");
   config.read(file);   
   file.close();
   
   // Try to connect with previous settings:
   WiFiManager wifiManager;
   wifiManager.setAPCallback(configModeCallback);
   wifiManager.setSaveConfigCallback(configSaveCallback);
   wifiManager.setDebugOutput(true);
   wifiManager.setBreakAfterConfig(true);

   WiFiManagerParameter applianceParam ("appliance", "Appliance Name", config["appliance"].c_str(), 32);
   WiFiManagerParameter timeServerParam ("timeserver", "Time (NTP) Server", config["timeserver"].c_str(), 40);
   WiFiManagerParameter timeOffsetParam ("timeoffset", "Time Offset (hours)", config["timeoffset"].c_str(), 10);
       
   //fetches ssid and pass from eeprom and tries to connect
   //if it does not connect it starts an access point with the specified host name
   //and goes into a blocking loop awaiting configuration
   ticker.attach(0.5, toggleLED);
   wifiManager.addParameter(&applianceParam);
   wifiManager.addParameter(&timeServerParam);   
   wifiManager.addParameter(&timeOffsetParam);   

   // Test the WIFI_RESET pin:
   if (digitalRead(WIFI_RESET) == 0)
      wifiManager.resetSettings();
   else      
      wifiManager.setConnectTimeout(60);

   // Attempt to connect. The appliance will wait up to 60 seconds if it has credentials and is not being reset.
   // This allows a reasonable amount of time for a network to recover after a power cut.
   wifiManager.autoConnect(host, NULL);

    // Store the new configation:
   config["appliance"] = applianceParam.getValue();
   config["timeserver"] = timeServerParam.getValue();
   config["timeoffset"] = timeOffsetParam.getValue();
   file = SPIFFS.open("/config.dat", "w");
   config.write(file);
   file.close();
    
   if (!WiFi.isConnected()) {
      Serial.println("failed to connect and hit timeout");
      //reset and try again - this gives it a 'clean start' at connecting with the new credentials.
      ESP.restart();
      delay(1000);
   }
    
   ticker.detach();    
   Serial.println("");
   Serial.print("Connected with IP address: ");
   Serial.println(WiFi.localIP());
   
   // Process the configuration:
   const char *timeServer = config["timeserver"].c_str();
   long timeOffset = config["timeoffset"].toInt()*3600;
   long dayLightOffset = 0; // DST is not yet supported in the ESP8266 core.
   configTime(timeOffset, dayLightOffset, timeServer, NULL, NULL);

   Serial.println("Appliance="+config["appliance"]);
   Serial.println("TimeServer="+String(timeServer));   
   Serial.println("TimeOffset="+String(timeOffset));   

   // Put the Arduino into reset and clear the serial stream.
   // This clears any junk that the appliance has sent prior to being put into reset.
   avrprog.setReset(true);
   while (Serial.available())
       Serial.read();

   // Start the remote debugger:
   remoteDebug.setResetCmdEnabled(true);
   remoteDebug.begin(host);
   Serial.println("Waiting for remote debug...");
   for (int i=0; i<10 && !remoteDebug.isActive(); ++i)
    { digitalWrite(BUILTIN_LED, HIGH);
      delay(900);
      remoteDebug.handle();
      digitalWrite(BUILTIN_LED, LOW);
      delay(100);
      remoteDebug.handle();
    }
      
   // Zeroconf Service announcements:
   // TBD instanace and host name can be extracted from panel.svg if we have some http parsing code, or other means to know the panel name.
   MDNS.begin(host);
   MDNS.setInstanceName(config["appliance"]);   
   MDNS.addService("ftp", "tcp", 21);
   MDNS.addService("avrisp", "tcp", AVR_PORT);
      
   // ICP server:
   avrprog.begin();

   // FTP Server
   ftpServer.begin("proteus","esp8266");    //username, password for ftp.  set ports in ESP8266FtpServer.h  (default 21, 50009 for PASV)
   ftpServer.setProgressHandler(&toggleLED);

   // Let the Arduino out of reset:
   Serial.println("Releasing AVR..."); 
   delay(100);
   Serial.setDebugOutput(false);
   avrprog.setReset(false);

   // Wait for the Arduino to start up:
   startBridge();   
}
 
void loop(void) {
   // Check for ISP connection/state change:
   static AVRISPState_t last_prog_state = AVRISP_STATE_IDLE;
   AVRISPState_t new_prog_state = avrprog.update();
   static bool last_ftp_state = false;
   bool new_ftp_state;
   
   if (last_prog_state != new_prog_state) {
      switch (new_prog_state) {
         case AVRISP_STATE_IDLE: {
            debug.print("[AVRISP] now idle\r\n");
            ticker.detach();
            startBridge();
            break;
         }
         case AVRISP_STATE_PENDING: {
            debug.print("[AVRISP] connection pending\r\n");
            bridge.end();
            break;
         }
         case AVRISP_STATE_ACTIVE: {
            debug.print("[AVRISP] programming mode\r\n");
            ticker.attach(0.1, toggleLED);
            break;
         }
      }
      last_prog_state = new_prog_state;
   }
    
   // Serve the client
   if (last_prog_state != AVRISP_STATE_IDLE) 
      avrprog.serve();
   else if (ftpServer.handleFTP()) {
      digitalWrite(BUILTIN_LED, HIGH);
      if (!last_ftp_state) {
         debug.println("[FTP] connected.");
         Serial.setDebugOutput(true);   
         last_ftp_state = true;
      }
      yield();
   } else if (last_ftp_state) {
      debug.println("[FTP] disconnected.");
      checkFirmware();
      Serial.setDebugOutput(false);   
      last_ftp_state = false;
   }  else {
      if (bridge.poll())
         digitalWrite(BUILTIN_LED, LOW);
      remoteDebug.handle();   
   }
}

void configModeCallback(WiFiManager *m) {
  Serial.println("Entered config mode");
  Serial.println(WiFi.softAPIP());
  //Serial.println(wiFiManager->getConfigPortalSSID());
  //entered config mode, make led toggle faster
  ticker.attach(0.2, toggleLED);
}

void configSaveCallback () {
  // When we get a new station config, we restart.
  // This seems to resolve various problems with dropped blocks.
  //ESP.restart();
  // delay(1000);
}

void startBridge ()
 { // Start the bridge, and wait for 'run-bridge' from the Arduino.
   bool started = false;
   debug.println("Attempting to start bridge...");
   for (int i=0; i<10 && !started; ++i) {
       digitalWrite(BUILTIN_LED, LOW);
       started = bridge.begin();
       remoteDebug.handle();
       digitalWrite(BUILTIN_LED, HIGH);
       delay(100);
   }
   if (started)
      debug.println("Bridge started.");
   else   
      debug.println("Bridge NOT started.");
 }
 
void checkFirmware () {
  // Open the firmware checksum:
  File md5 = SPIFFS.open("/firmware.md5", "r");
  char md5str[33];
  if (md5.size() != 16)
      return;
  for (uint8_t i = 0; i < 16; i++) 
     sprintf(md5str + (i * 2), "%02x", md5.read());

  // If this is the same as the current sketch, then do nothing. 
  if (ESP.getSketchMD5() == md5str) {
     debug.println("Firmware is unchanged.");
     return; 
  }
  
  // Open the firmware file:
  File firmware = SPIFFS.open("/firmware.bin", "r");
  if (firmware.size() == 0)  {
      debug.println("Firmware binary not found.");
      return;
  }
  
  // Attempt to re-flash ourselves:       
  Update.setMD5(md5str);
  if (Update.begin(firmware.size())) {
      debug.println("Updating firmware...");
      debug.stop();
      Serial.println("\nUpdating firmware...\n");      
      Update.write(firmware);
      Update.end();
      ESP.restart();
      delay(1000);
  }  
}     


void toggleLED () {
   digitalWrite(BUILTIN_LED, !digitalRead(BUILTIN_LED));   
}



