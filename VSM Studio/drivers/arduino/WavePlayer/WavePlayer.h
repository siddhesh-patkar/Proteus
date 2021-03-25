#pragma once

#include <arduino.h>
#include <core.h>

#include <utility\WaveHc.h>

class WavePlayer : public WaveHC
 { public:
      void begin(uint8_t cs, uint8_t sck, uint8_t sdi, uint8_t ldac);
      bool operator() () { return isplaying; }
      void play (const String &s, bool wait) { play(s.c_str(), wait); }
      void play (const char *filename, bool wait);     
   private:
      FatReader file;
 };

extern WavePlayer WP;