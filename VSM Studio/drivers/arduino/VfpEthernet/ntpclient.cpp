/********************************************************************/
/********************************************************************/
/*****                                                          *****/
/*****        L A B C E N T E R    E L E C T R O N I C S        *****/
/*****                                                          *****/
/*****                 Visual Designer for Arduino              *****/
/*****                                                          *****/
/*****              Virtual Front Panel (Ntp Client)            *****/
/*****                                                          *****/
/********************************************************************/
/********************************************************************/

#include <Arduino.h>
#include <Ethernet.h>
#include <EthernetUdp.h>
#include <Dns.h>
#include "NTPClient.h"

void NTPClient::begin (const char *host, int8_t offset_hrs)
 { DNSClient dns;  
   dns.begin(Ethernet.dnsServerIP());
   dns.getHostByName(host, serverIP);
   localOffset = 3600L * offset_hrs;
 }

bool NTPClient::getLocalTime (unsigned long &epoch, uint16_t timeout)
// Return the network time converted to local time as specified by the local time offset.
// This could be updated to understand daylight savings although arguably this should be done in the context of the time library.
 { bool flag = getNetworkTime(epoch, timeout);
   if (flag)
      epoch += localOffset;
   return flag;   
 } 

bool NTPClient::getNetworkTime (unsigned long &epoch, uint16_t timeout)
// Return the network time converted to Unix time (GMT)
 { uint32_t startTime = millis();

   // Find a socket to use
   while (EthernetUDP::begin(1024+(millis() & 0xF)) != 1)
    { if ((millis() - startTime) > timeout)
         return false;
      delay(1);
    }
   
   // send an NTP packet to the time server
   sendNTPpacket(); 
   
   // Wait for a response packet
   while(parsePacket() <= 0)
    { if ((millis() - startTime) > timeout)
       { stop();
         return false;          
       }
      delay(50);
    }

    // We've received a packet, read the data from it
    read(packetBuffer, NTP_PACKET_SIZE); // read the packet into the buffer
    
    // Done with the UDP socket:
    stop();

    // the timestamp starts at byte 40 of the received packet and is four bytes,
    // or two words, long. First, extract the two words:

    unsigned long highWord = word(packetBuffer[40], packetBuffer[41]);
    unsigned long lowWord = word(packetBuffer[42], packetBuffer[43]);
    // combine the four bytes (two words) into a long integer
    // this is NTP time (seconds since Jan 1 1900):
    unsigned long secsSince1900 = highWord << 16 | lowWord;
    //Serial.print("Seconds since Jan 1 1900 = ");
    //Serial.println(secsSince1900);
   
    // Unix time starts on Jan 1 1970. In seconds, that's 2208988800:
    const unsigned long seventyYears = 2208988800UL;
    // subtract seventy years:
    epoch = secsSince1900 - seventyYears;

    //Serial.print("Unix time = ");
    //Serial.println(epoch);
   
   return true;
   
 }
  

// send an NTP request to the time server at the given address
void NTPClient::sendNTPpacket ()
 { 
    { // set all bytes in the buffer to 0
      memset(packetBuffer, 0, NTP_PACKET_SIZE);
      // Initialize values needed to form NTP request
      // (see URL above for details on the packets)
      packetBuffer[0] = 0b11100011;   // LI, Version, Mode
      packetBuffer[1] = 0;     // Stratum, or type of clock
      packetBuffer[2] = 6;     // Polling Interval
      packetBuffer[3] = 0xEC;  // Peer Clock Precision
      // 8 bytes of zero for Root Delay & Root Dispersion
      packetBuffer[12]  = 49;
      packetBuffer[13]  = 0x4E;
      packetBuffer[14]  = 49;
      packetBuffer[15]  = 52;  
   
      // all NTP fields have been given values, now
      // you can send a packet requesting a timestamp:
      beginPacket(serverIP, 123); //NTP requests are to port 123
      write(packetBuffer, NTP_PACKET_SIZE);
      endPacket();
      
    }
 }