#include "core.h"
#include "vfpserver.h"
#include "filestore.h"
#include "controls.h"

TextLog::TextLog (const char *_id) : Control(_id) 
 { recording = false;
 }

void TextLog::setFile (const String &file) 
 { if (recording)
    { VFP.erase(id);
      VFP.record(id, false);
      recording = false;
    }
   VFP.set(id, F("file")).arg(file).end();  
   fileName = file;
   delay(2000); // Give any attached client time to load the file
 }

PrintList &TextLog::info() { return begin(false, &text); }
PrintList &TextLog::warning() { return begin(true, &text).arg(F("w\r")); }
PrintList &TextLog::error() { return begin(true, &text).arg(F("e\r")); }
      
void TextLog::clear() 
 { VFP.call(id, F("clear")).end(); 
   VFP.erase(id);
   FS.remove(fileName);
 }
      
void TextLog::end() 
 { if (fileName != NULL)
    { Handle fh = FS.open(fileName, FILE_APPEND);
      FS.println(fh).arg(text.buffer()).end();
      FS.close(fh);
    }
   else if (!recording)
    { VFP.record(id, true);
      recording = true;
    } 
   VFP.call(id, F("appendLine")).arg(text.buffer()).end(); 
   text.clear(); 
 }