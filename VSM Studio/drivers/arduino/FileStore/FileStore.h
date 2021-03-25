// Wrapper for the SD/FAT file storage library

#ifndef __FILESTORE_H__
#define __FILESTORE_H__

#pragma once

#include <core.h>
#include <sd.h>

class FileStore
 { public:
      bool begin(uint8_t cs);
      void uploader(uint8_t cs);
      bool operator()(const String &filename);
      Handle open(Handle &h);
      Handle open(const String &filename, uint8_t mode);
      void close(Handle &h);
      void remove(const String &filename);
      void mkdir(const String &dirname);
      void rmdir(const String &dirname);
      void flush(Handle &h);
      StreamList &read(Handle &h);
      String readln(Handle &h);
      PrintList &write(Handle &h);
      PrintList &writeln(Handle &h);
      PrintList &print(Handle &h);
      PrintList &println(Handle &h);
      void setDelimiter(char d) { readList.setDelimiter(d); writeList.setDelimiter(d); }
      void setPlaces (int p) { printList.setPlaces(p); }
      void setBase(int b) { printList.setBase(b); }
   private:
      void calcCRC(byte &crc, byte value);
      void writeLong(unsigned long value);
   private:
      StreamList readList;
      PrintList writeList, printList;
 };       

inline bool eof(Handle &h) { return h.type != H_FILE || !((SDLib::File *)h.ptr)->available(); }

extern FileStore FS;
extern void FSdateTime (uint16_t* date, uint16_t* time);

#endif