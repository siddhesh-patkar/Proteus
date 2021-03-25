/********************************************************************/
/********************************************************************/
/*****                                                          *****/
/*****        L A B C E N T E R    E L E C T R O N I C S        *****/
/*****                                                          *****/
/*****                  VFP Server for ESP8266                  *****/
/*****                                                          *****/
/*****               PacketReader & CRC Computation             *****/
/*****                                                          *****/
/********************************************************************/
/********************************************************************/

#include "bridge.h"
#include "packet.h"
#include "command.h"

PacketReader::PacketReader (Stream &stream)
            : transport(stream)
 { index = 999;    
   last_response_valid = false;
 }

bool PacketReader::process (CommandProcessor *cp)
// Top level of the command processing.
 { // Do a round of the runners:
   cp->run();
   
   // Wait for Start Of Packet which must be 0xFF
   int c;
   while (true)
    { // Try to read a character - drop out if none.
      if (!transport.available())
         return false;
      // Look for an 0xFF - this is the start of a packet:   
      if (c = t_read(), c == 0xFF)  
         break;
    }
          
   // Read index and length:
   int index, len_hi, len_lo;
   if ((index = t_read()) == -1)
      return false;;
   if ((len_hi = t_read()) == -1)
      return false;
   if ((len_lo = t_read()) == -1)
      return false;

   // Start to build a CRC:
   CRC crc;
   crc.write(c);
   crc.write(index);
   crc.write(len_hi);
   crc.write(len_lo);
   
   // Compute the total length
   int len_t = ((len_hi << 8) + len_lo);

 
   // Read payload
   String data;
   for (int i=0; i<len_t; ++i)
    { int c = t_read();
      if (c == -1)
         return false;
      data += char(c);
      crc.write(c);
    }
   
   // Read and check CRC
   int crc_hi, crc_lo;
   if ((crc_hi = t_read()) == -1)
      return false;
   if ((crc_lo = t_read()) == -1)
      return false;       
   
   if (!crc.check((crc_hi << 8) + crc_lo))
      return false;
   
   // Check for reset command - the index gets reset here.
   if (data.length() == 5 && data.startsWith("XX"))
      this->index = index;
  
   // Check for out-of-order packets
   if (this->index != index)
    { if (last_response_valid)
         send(index, last_response);
      return true;
    }
   
   // Process command
   String result = cp->process(data);
   
   // Send Acknowledge
   send(this->index, result);
   this->index = (this->index + 1) & 0xFF;
   last_response = String();
   for (int i=0; i<result.length(); ++i)
      last_response += result[i];
   last_response_valid = true;
 
   return true;
 } 

int PacketReader::t_read ()
// Timed read from the transport with 50ms timeout.
// The assumption is that the Arduino will send packets atomically.
 { int c;
   unsigned long _startMillis = millis();
   do {
       c = transport.read();
       if (c >= 0) return c;
       // yield();
      } while (millis() - _startMillis < 50);
   return -1;     // -1 indicates timeout
 }

void PacketReader::send (int index, const String &msg)
// Send a Packet
 { CRC crc (&transport);
   crc.write(0xFF);     // Packet start
   crc.write(index);    // Message No. inside Pipe
   int l = msg.length();
   crc.write(l >> 8);   // Message length
   crc.write(l & 0xFF); // Message length
   crc.write(msg);      // Payload
   crc.write_crc();     // CRC
 }
 
/*********************************************************************
**** Class CRC ****
******************/

CRC::CRC (Stream *_transport)
   : transport(_transport)
 { result = 0xFFFF;
 }   
      
void CRC::write (const String &data)
 { if (transport != NULL)
      transport->print(data);
   for (int i=0; i<data.length(); ++i)        
      update(data[i]);
 }
 
void CRC::write_crc()
 { if (transport != NULL)
    { String data;
      data += char(result >> 8);
      data += char(result & 0xFF);
      transport->print(data);
    }
 }  

void CRC::update(uint8_t data)
 { uint16_t crc = result;
   uint8_t hi8;
   data ^= crc;
   data ^= (data << 4);
   hi8 = (crc >> 8) & 0xFF;
   result = ((((uint16_t)data << 8) | hi8) ^ (data >> 4) ^ ((uint16_t)data << 3));   
 }
 
