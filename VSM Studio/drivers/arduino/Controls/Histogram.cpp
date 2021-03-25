/********************************************************************/
/********************************************************************/
/*****                                                          *****/
/*****        L A B C E N T E R    E L E C T R O N I C S        *****/
/*****                                                          *****/
/*****                    Proteus IoT Builder                   *****/
/*****                                                          *****/
/*****                     Histogram Control                    *****/
/*****                                                          *****/
/********************************************************************/
/********************************************************************/

#include "vfpserver.h"
#include "filestore.h"
#include "controls.h"

Histogram::Histogram (const char *id) : Control (id)
 { setDelimiter(',');
   recording = false;
 }

void Histogram::showColumn (uint8_t column, bool visible)
 { String key = F("ColumnVisible");
   key += String(column);
   VFP.set(id, key).arg(visible).end();
 }

void Histogram::setYRange (float min, float max)
 { VFP.set(id, F("yRange")).arg(min).arg(max).end();
 }

void Histogram::setDataFile (const String &file)
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

void Histogram::clearData()
 { VFP.call(id, F("clearData")).end();
   VFP.erase(id);
   FS.remove(fileName);
 }

PrintList &Histogram::writeData ()
// Append data to the graph. If recording is enabled, the server persists these calls as part of the session history.
 { return begin(false, &data);
 }

void Histogram::end()
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

