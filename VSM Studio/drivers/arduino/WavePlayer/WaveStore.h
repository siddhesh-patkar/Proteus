#pragma once

#include <arduino.h>
#include <core.h>

#include <utility\FatReader.h>

class WaveStore 
 { public:
      void begin(uint8_t cs);

      // Public API:
      bool operator() (const char *fileName);            
      void changeDir(const char *dirName);
      String getFirstFile();
      String getNextFile();
      String getFirstDir();
      String getNextDir();      
      
      // Used by WavePlayer      
      bool open(const char *fileName, FatReader &reader) { return reader.open(cwd, (char *)fileName); }

   private:
      SdReader card;  
      FatVolume vol;   
      FatReader cwd;
      
 
 };

extern WaveStore WS;