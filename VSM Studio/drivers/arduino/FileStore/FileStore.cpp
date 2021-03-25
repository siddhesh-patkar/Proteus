/********************************************************************/
/********************************************************************/
/*****                                                          *****/
/*****        L A B C E N T E R    E L E C T R O N I C S        *****/
/*****                                                          *****/
/*****                 Visual Designer for Arduino              *****/
/*****                                                          *****/
/*****              Wrapper for SD Card/FAT File Store          *****/
/*****                                                          *****/
/********************************************************************/
/********************************************************************/

#include <spi.h>
#include <sd.h>
#include "filestore.h"

const char __ATTR_PROGMEM__ H_RESOURCE[] = "H_RESOURCE";
const char __ATTR_PROGMEM__ H_FILE[] = "H_FILE";

FileStore FS;

bool FileStore::begin(uint8_t cs)
// Initialize the SD card storage module.
 { pinMode(cs, OUTPUT);
   setDelimiter(',');
   SdFile::dateTimeCallback(&FSdateTime);
   return SD.begin(cs); 
 }

bool FileStore::operator()(const String &filename)
// Return true if file exists.
 { return SD.exists((char *)filename.c_str());
 }

Handle FileStore::open (Handle &h)
// Return a read only File Handle a resource file.
 { Handle fh;
   if (h.type == H_RESOURCE)
      h = open(h.resource, FILE_READ);
   return fh; 	 	  	
 }

Handle FileStore::open (const String &filename, uint8_t mode)
// Open a file.
 { File file = SD.open(filename.c_str(), mode);
 	 Handle fh;
 	 if (file)
 	 	{ fh.type = H_FILE;
 	 	  fh.file = new File(file);
 	 	}
 	 else
 	 		fh.type = NULL, fh.ptr = NULL;
 	 return fh;		 	 		
 } 	 			 	 	
 	 	   	  	   
void FileStore::close(Handle &fh)
// Close a file.
 { if (fh.type == H_FILE)
 	  {fh.type = NULL;
 	  	fh.file->close();
 	  	delete fh.file; 	  	
      fh.file = NULL;
 	  }
 }

void FileStore::remove(const String &filename)
// Remove a file or directory
 { SD.remove((char *)filename.c_str());
 } 
 
void FileStore::mkdir(const String &dirname)
// Make a new folder
 { SD.mkdir((char *)dirname.c_str());
 }
 
void FileStore::rmdir(const String &dirname)
// Remove a folder.
 { SD.rmdir((char *)dirname.c_str());
 }

void FileStore::flush(Handle &h)
// Flush.
 { h.file->flush();
 }

StreamList &FileStore::read(Handle &h) 
// Read items separated by the specified delimiter.
 { return readList.begin(h.type == H_FILE ? h.file : NULL);  
 }


String FileStore::readln (Handle &h)
 { String s;
   if (h.type == H_FILE && h.file != NULL)
    { s = h.file->readStringUntil('\r');
      h.file->read(); // skip the 'n';
    }
   return s;          
 }
  
PrintList &FileStore::write(Handle &h)   
 { return writeList.begin(false, h.type == H_FILE ? h.file  : NULL); 
 }

PrintList &FileStore::writeln(Handle &h)   
 { return writeList.begin(true, h.type == H_FILE ? h.file  : NULL); 
 }

PrintList &FileStore::print(Handle &h)   
 { return  printList.begin(false, h.type == H_FILE ? h.file  : NULL); 
 }

PrintList &FileStore::println(Handle &h) 
 { return printList.begin(true, h.type == H_FILE ? h.file  : NULL); 
 }

#define UPLOADER_SPEED 115200
#define UPLOADER_NAME_SIZE 14
#define UPLOADER_BLOCK_SIZE 32 
#define UPLOADER_MAX_TIMEOUT 1000

enum UploaderState
 { UploaderUninitialized,
   UploaderInitializing,
   UploaderInitialized,
   UploaderReady,
   UploaderReceivingName,
   UploaderReceivingSize,
   UploaderReceivingData,
   UploaderReceivingCRC,
   UploaderFinished
 };

void FileStore::calcCRC(byte &crc, byte value)
 { for (byte j = 8; j>0; j--)
    { byte sum = (crc ^ value) & 0x01;
      crc >>= 1;
      if (sum)
         crc ^= 0x8C;
      value >>= 1;
    }
 }

void FileStore::writeLong(unsigned long value)
 { Serial.write((value & 0x000000FF));
   Serial.write((value & 0x0000FF00) >> 8);
   Serial.write((value & 0x00FF0000) >> 16);
   Serial.write((value & 0xFF000000) >> 24);
 }

void FileStore::uploader(uint8_t cs)
 { UploaderState state = UploaderUninitialized;
   char name[UPLOADER_NAME_SIZE];
   unsigned long size=0;
   unsigned long blockSize;
   int pos=0;
   byte crc=0;
   int timeout = 0;
   File f;

   // Serial port speed
   Serial.begin(UPLOADER_SPEED);
                     
   while (state != UploaderFinished)
    { if (Serial.available())
       { timeout=0;
         byte value = Serial.read();
         switch (state)
          { // Initialization sequence
            case UploaderUninitialized:
               if (value == 'R')
                { Serial.write('X');
                  state = UploaderInitializing;
                }
               break;
            case UploaderInitializing:
               if (value == 'r')
                { Serial.write('x');
                  state = UploaderInitialized;
                }
               else
                  state = UploaderUninitialized;
               break;
            case UploaderInitialized:
               if (value == 'I')
                { // see if the card is present and can be initialized:
                  if (begin(cs))
                   { state = UploaderReady;
                     Serial.write('K');
                   }
                  else
                   { state = UploaderFinished;
                     Serial.write('F');
                   }
                }
               break;
            case UploaderReady:
               if (value == 'U')
                { // File upload initiated
                  Serial.write('N');
                  pos = 0;
                  state = UploaderReceivingName;
                }
               else if (value == 'E')
                { // Finished
                  state = UploaderFinished;
                }
               break;
            case UploaderReceivingName:
               if (pos == UPLOADER_NAME_SIZE)
                { // File name doesn't fit into the buffer
                  state = UploaderUninitialized;
                  break;
                }
               name[pos++] = value;
               if (value == 0)
                { // File name received
                  pos = 0;
                  size = 0;
                  // Open file
                  SD.remove(name);
                  f = SD.open(name, FILE_WRITE);
                  if (f)
                   { // Request file size
                     Serial.write('S');
                     state = UploaderReceivingSize;
                   }
                  else
                   { // Failed to open file
                     Serial.write('F');
                     state = UploaderUninitialized;
                   }
                }
               break;
            case UploaderReceivingSize:
               size += ((unsigned long)value) << (8*(pos++));
               if (pos == 4)
                { // Send block size
                  crc = 0;
                  pos = 0;
                  blockSize = UPLOADER_BLOCK_SIZE;
                  if (size < blockSize)
                     blockSize = size;
                  writeLong(blockSize);
                  state = UploaderReceivingData;
                }
               break;
            case UploaderReceivingData:
               // Next data byte received
               f.write(value);
               calcCRC(crc, value);
               size--;
               pos++;
               if (pos == UPLOADER_BLOCK_SIZE || size == 0)
                { Serial.write('C');
                  state = UploaderReceivingCRC;
                }
               break;
            case UploaderReceivingCRC:
               if (value != crc)
                { // CRC check failed
                  // Return to unitialized state
                  Serial.write('F');
                  state = UploaderUninitialized;
                  break;
                }
               if (size == 0)
                { // All data received
                  Serial.write('K');
                  state = UploaderReady;
                  f.close();
                  break;
                }
               crc = 0;
               pos = 0;
               // Size of the next block of data
               blockSize = UPLOADER_BLOCK_SIZE;
               if (size < blockSize)
                  blockSize = size;
               writeLong(blockSize);
               state = UploaderReceivingData;
               break;
            default:
               // Something happened
               state = UploaderFinished;
          }
       }
      else
       { delay(1);
         if (++timeout > UPLOADER_MAX_TIMEOUT)
            state = UploaderFinished;
       }
    }
    
   if (f)
      f.close();
 }


void FSdateTime (uint16_t* date, uint16_t* time)
// Use the time library to set file date/time stamps.
 { if (timeStatus() == timeSet)
    { time_t t = now();
      *date = FAT_DATE(year(t), month(t), day(t));
      *time = FAT_TIME(hour(t), minute(t), second(t));
    }
   else
    { *date = FAT_DEFAULT_DATE;
      *time = FAT_DEFAULT_TIME; 
    }
 }

