
#include <arduino.h>
#include <core.h>

#include "WavePlayer.h"
#include "WaveStore.h"

WavePlayer WP;
 
void WavePlayer::begin(uint8_t cs, uint8_t sck, uint8_t sdi, uint8_t ldac)
// Currently the hardware is hard coded.
 { isplaying = false;
 }
       
void WavePlayer::play (const char *fileName, bool wait)
// Play a named WAV file.
 { if (isplaying)
      stop();      
   if (!WS.open(fileName, file))
    { Serial.print("Could not open file: ");
      Serial.println(fileName);
    }
   else if (!create(file))
    { Serial.print("Not a valid WAV file: ");
      Serial.println(fileName);
    }      
   else
    { WaveHC::play();
      if (wait)         
         while (isplaying)
            ;
    }   
 } 