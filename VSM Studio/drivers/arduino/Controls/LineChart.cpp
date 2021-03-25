#include "core.h"
#include "vfpserver.h"
#include "filestore.h"
#include "controls.h"

LineChart::LineChart (const char *id) : Control (id) 
 { setDelimiter(','); 
   recording = false;
 }
   
void LineChart::showColumn (uint8_t column, bool visible) 
 { String key = F("ColumnVisible");
   key += String(column);
   VFP.set(id, key).arg(visible).end(); 
 }
 
void LineChart::setYrange (uint8_t axis, float min, float max) 
 { String key = F("YRange");
   //key += String(axis);
   VFP.set(id, key).arg(axis).arg(min).arg(max).end(); 
 }        
   
void LineChart::setDataFile (const String &file) 
// If a file is loaded, any session data is erased and appendData does not record since the file data will be
// reloaded on restore; appenData is only used if the graph is to update dynamically.
 { if (recording)
    { VFP.erase(id);
      VFP.record(id, false);
      recording = false;
    }
   VFP.set(id, F("data")).arg(file).end();  
   fileName = file;
 }

void LineChart::clearData() 
 { VFP.call(id, F("clearData")).end(); 
   VFP.erase(id);
   FS.remove(fileName);
 }
      
PrintList &LineChart::writeData ()
// Append data to the graph. If recording is enabled, the server persists these calls as part of the session history.
 { return begin(false, &data); 
 }


void LineChart::end() 
 { if (fileName != NULL)
    { Handle fh = FS.open(fileName, FILE_APPEND);
      FS.println(fh).arg(data.buffer()).end();
      FS.close(fh);
    }
   else if (!recording)
    { VFP.record(id, true);
      recording = true;
    } 
   VFP.call(id, F("appendData")).arg(data.buffer()).end(); 
   data.clear(); 
 }
  