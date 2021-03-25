/********************************************************************/
/********************************************************************/
/*****                                                          *****/
/*****        L A B C E N T E R    E L E C T R O N I C S        *****/
/*****                                                          *****/
/*****                    Proteus IoT Builder                   *****/
/*****                                                          *****/
/*****                     Wind Rose Control                    *****/
/*****                                                          *****/
/********************************************************************/
/********************************************************************/

#include "vfpserver.h"
#include "filestore.h"
#include "controls.h"
#include "math.h"

WindRose::WindRose (const char *id) : Control (id) 
 { clearRecord();
 }
   
void WindRose::begin (const String &fn, const String &bands) 
 { int8_t i=0, s=0, e;
   fileName = fn;
   do 
    { e = bands.indexOf(',', s);
      thresholds[i++] = bands.substring(s, e).toInt();
      s = e+1;
    } while (e != -1 && i<WIND_BANDS-1);         
 }   

void WindRose::setTimeRange (uint8_t unit, long range)
 { VFP.set(id, F("TimeRange")).arg(unit).arg(range).end(); 
 }
 
void WindRose::storeReading (float wind_dir, float wind_speed)
// Store a wind reading in the appropriate bin.   
 { uint8_t dir = round (WIND_DIRS*wind_dir/360.0) % WIND_DIRS;
   uint8_t band = 0;
   while (band < WIND_BANDS-1 && wind_speed > thresholds[band])
      band++;
   if (band == 0)
      calm += 1;
   else      
      bins[dir][band-1] += 1;
 }

void WindRose::storeRecord ()
// Write a wind record to the data file.   
// The record consists of 1 line per wind direction with each line containing the accumulated intervals for each band.
// The assumption is that this will be called fairly infrequently and so we update any attached control after each update.
 { Handle fh = FS.open(fileName, FILE_APPEND);
   FS.print(fh).arg(now()).arg(F(",")).arg(calm).end();
   for (uint8_t i=0; i<WIND_DIRS; ++i)      
    { PrintList &list = FS.print(fh);
      for (uint8_t j=0; j<WIND_BANDS-1; ++j)
         list.arg(F(",")).arg(bins[i][j]);
      list.end();
    }
   FS.println(fh).end();      
   FS.close(fh);
   clearRecord();
   VFP.call(id, F("loadData")).end();
 }

void WindRose::clearRecord ()
// Start a new record.
 { memset(bins, 0, sizeof(bins));
   calm = 0;
 }

void WindRose::clearData() 
 { VFP.call(id, F("clearData")).end(); 
   FS.remove(fileName);
 }
      

