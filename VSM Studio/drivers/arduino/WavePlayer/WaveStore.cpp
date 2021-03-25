
#include <arduino.h>
#include <core.h>

#include "WaveStore.h"

WaveStore WS;

void WaveStore::begin (uint8_t ss)
// Initialize the local SD subsystem.
// SS pin  is passed but is actually hard coded in WavePinDefs.h  
 { if (!card.init())
    { Serial.print("Failed to initialize SD card. Error Code:");
      Serial.println(card.errorCode(), DEC);
      return;
    }
    
   // enable optimize read - some cards may timeout. 
   card.partialBlockRead(true);
  
   // Now we will look for a FAT partition!
   uint8_t part;
   for (part = 0; part < 5; part++)     // we have up to 5 slots to look in
      if (vol.init(card, part)) 
       { cwd.openRoot(vol);
         break;                          // we found one, lets bail
       }             
 }

bool WaveStore::operator() (const char *fileName)
// Return true if the named file exists.
 { FatReader file;
   return file.open(cwd, (char *)fileName)!=0;
 }   
      
void WaveStore::changeDir(const char *dirName)
// Attempt to change to a new folder.
 { FatReader newDir;
   if (newDir.open(cwd, (char *)dirName) && newDir.isDir())
      cwd = newDir;      
 }

String WaveStore::getFirstFile ()
// Return first file name in the cwd.
 { cwd.rewind();
   return getNextFile();
 }

String WaveStore::getNextFile()
// Return the next file name in the cwd.
 { String s;
   dir_t entry;
   char fileName[13];
   while (cwd.readDir(entry))
      if (DIR_IS_FILE(entry))
       { dirName(entry, fileName);
         s = fileName;
         break;
       }
   return s;         
 }         
      
String WaveStore::getFirstDir ()
// Return first sub-directory name in the cwd.
 { cwd.rewind();
   return getNextDir();   
 }

String WaveStore::getNextDir()
// Return the next sub-directory name in the cwd.
 { String s;
   dir_t entry;
   char subdirName[13];
   while (cwd.readDir(entry))
      if (DIR_IS_SUBDIR(entry))
       { dirName(entry, subdirName);
         s = subdirName;
         break;
       }
   return s;
 }         
