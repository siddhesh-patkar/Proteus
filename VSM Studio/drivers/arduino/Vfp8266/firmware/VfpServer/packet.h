/********************************************************************/
/********************************************************************/
/*****                                                          *****/
/*****        L A B C E N T E R    E L E C T R O N I C S        *****/
/*****                                                          *****/
/*****                  VFP Server for ESP8266                  *****/
/*****                                                          *****/
/*****                  Bridge / Packet Reader                  *****/
/*****                                                          *****/
/********************************************************************/
/********************************************************************/

#pragma once

class CommandProcessor;

class PacketReader 
 { public:
      PacketReader (Stream &stream);

      bool process(CommandProcessor *cp);

   private:       
      int t_read ();
      void send(int index, const String &data);
   
   private:   
      CommandProcessor *processor;
      Stream &transport;      
      String last_response;
      bool last_response_valid;
      int index;
          
 };
 
class CRC
 { public:
      CRC (Stream *transport=NULL);
      
      void write (char c) { String data; data += (char)c; write(data); }
      void write (const String &data);
      void write_crc ();
      bool check (uint16_t crc) { return result == crc;}
      int crc() { return result; }

   private:      
      void update(uint8_t data);
 
   private:
      Stream *transport;
      uint16_t result;
 };
