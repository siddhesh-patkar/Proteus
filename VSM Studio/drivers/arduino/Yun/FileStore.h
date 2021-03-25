// Wrapper for the Yun FileIO library

#ifndef __FILESTORE_H__
#define __FILESTORE_H__

#pragma once

#include <FileIO.h>

namespace Yun 
 { class FileStore 
    { public:
         FileStore();
         bool begin();
         bool operator()(const String &filename);
         Handle open(Handle &h);
         Handle open(const String &filename, uint8_t mode);
         void close(Handle &h);
         void remove(const String &filename);
         void chdir(const String &dirname);
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
         BridgeLib::File *file(Handle &h) { return (BridgeLib::File *)h.ptr;  }
         String absPath(const String &filename) { return filename[0] == '/' ? filename : cwd+filename; }
      private:
         bool vsm;
         String cwd;
         StreamList readList;
         PrintList writeList, printList;
    };       
 }    

inline bool eof(Handle &h)  { return h.type != H_FILE || !((BridgeLib::File *)h.ptr)->available(); }

extern Yun::FileStore FS;


#endif