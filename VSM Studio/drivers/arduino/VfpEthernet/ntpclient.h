// Arduino NTP client for WizNet5100-based Ethernet shield

#pragma once

#ifndef NTPClient_h
#define NTPClient_h

#include <EthernetUdp.h>

const int NTP_PACKET_SIZE = 48; // NTP time stamp is in the first 48 bytes of the message
const int NTP_TIMEOUT  = 1000;     // Timeout for requests.

class NTPClient : private EthernetUDP
 { public:       
      void begin (const char *host, int8_t offset_hrs);  
      bool getLocalTime (unsigned long& aResult, uint16_t timeout=NTP_TIMEOUT);
      bool getNetworkTime (unsigned long& aResult, uint16_t timeout=NTP_TIMEOUT);
      IPAddress getTimeServer() { return serverIP; }

   private:
      void sendNTPpacket();
   
   private:   
      IPAddress serverIP;
      byte packetBuffer[NTP_PACKET_SIZE]; //buffer to hold incoming and outgoing packets
      long localOffset;
 };

#endif
