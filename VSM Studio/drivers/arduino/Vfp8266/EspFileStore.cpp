/********************************************************************/
/********************************************************************/
/*****                                                          *****/
/*****        L A B C E N T E R    E L E C T R O N I C S        *****/
/*****                                                          *****/
/*****                 Visual Designer for Arduino              *****/
/*****                                                          *****/
/*****                FileStore for ESP8266 Flash/SD            *****/
/*****                                                          *****/
/********************************************************************/
/********************************************************************/

#include "FileStore.h"

using namespace Vfp8266;
using namespace BridgeLib;

const char PROGMEM H_RESOURCE[] = "H_RESOURCE";
const char PROGMEM H_FILE[] = "H_FILE";

FileStore FS;

FileStore::FileStore ()
 { cwd = F("/");
   setDelimiter(',');
 }

bool FileStore::begin()
// Start the bridge and filesystem libraries. 
 { FileSystem.begin(); 
 }


bool FileStore::operator()(const String &filename)
// Return true if file exists.
 { return FileSystem.exists((char *)absPath(filename).c_str());
 }

Handle FileStore::open (Handle &h)
// Return a read only File Handle a resource file.
 { Handle fh;
   if (h.type == H_RESOURCE)
      h = open(String(F("/"))+h.resource, FILE_READ);
   return fh; 	 	  	
 }

Handle FileStore::open (const String &filename, uint8_t mode)
// Open a file.
 { BridgeLib::File *file;
   file = new BridgeLib::File (absPath(filename).c_str(), mode);     
 	Handle fh;
 	if (*file)
 	 { fh.type = H_FILE;
 	   fh.ptr = file;
 	 }
 	else
 	 { fh.type = NULL, fh.ptr = NULL;
 	   delete file;
 	 }
 	return fh;		 	 		
  } 	 			 	 	
 	 	   	  	   
void FileStore::close(Handle &fh)
// Close a file.
 { if (fh.type == H_FILE)
 	 { BridgeLib::File *fp = file(fh);
 	   fh.type = NULL;
      fh.ptr = NULL;
 	  	fp->close();
 	  	delete fp; 	  	
    }
 }

void FileStore::remove(const String &filename)
// Remove a file or directory
 { FileSystem.remove((char *)absPath(filename).c_str());
 } 

void FileStore::chdir(const String &dirname)
 { cwd = absPath(dirname);
   cwd += '/';   
 }
 
void FileStore::mkdir(const String &dirname)
// Make a new folder
 { FileSystem.mkdir((char *)absPath(dirname).c_str());
 }
 
void FileStore::rmdir(const String &dirname)
// Remove a folder.
 { FileSystem.rmdir((char *)absPath(dirname).c_str());
 }

void FileStore::flush(Handle &h)
// Flush.
 { file(h)->flush();
 }

StreamList &FileStore::read(Handle &h) 
// Read items separated by the specified delimiter.
 { return readList.begin(h.type == H_FILE ? file(h) : NULL);  
 }


String FileStore::readln (Handle &h)
 { String s;
   BridgeLib::File *fp = file(h);
   if (h.type == H_FILE && fp != NULL)
    { s = fp->readStringUntil('\r');
      fp->read(); // skip the 'n';
    }
   return s;          
 }
  
PrintList &FileStore::write(Handle &h)   
 { return writeList.begin(false, h.type == H_FILE ? file(h)  : NULL); 
 }

PrintList &FileStore::writeln(Handle &h)   
 { return writeList.begin(true, h.type == H_FILE ? file(h)  : NULL); 
 }

PrintList &FileStore::print(Handle &h)   
 { return  printList.begin(false, h.type == H_FILE ? file(h)  : NULL); 
 }

PrintList &FileStore::println(Handle &h) 
 { return printList.begin(true, h.type == H_FILE ? file(h)  : NULL); 
 }
