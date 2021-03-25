#include "core.h"
#include "vfpserver.h"
#include "filestore.h"
#include "controls.h"

/***********************************************************************
 **** BarChart ****
 *****************/

// A series holds totals for each of the declared catagories for the
// BarChart.

BarChart::BarChart (const char *id)
: Control (id),
  catCount (-1),
  activeSeries (NULL)
 { setDelimiter(',');
 }

BarChart::~BarChart()
 { if (activeSeries != NULL)
    { free (activeSeries);
    }
 }

// This is used to obtain info from the js config. Here the memory for
// the series is allocated and initialised.
void BarChart::init (int _catCount)
 { catCount = _catCount;
   activeSeries = (float *) malloc (sizeof(float) * _catCount);
   for (uint8_t i = 0; i < catCount; ++i)
    { activeSeries[i] = 0.0f;
    }
 }

// Called on client-reconnect. Only properties not set with "VFP.set" need be refreshed.
void BarChart::reload()
 { for (uint8_t i = 0; i < catCount; ++i)
    { VFP.call(id, F("setData")).arg(0).arg(i).arg(activeSeries[i]).end();
    }
 }

// Show or hide a series.
void BarChart::setSeriesVisible (uint8_t seriesIdx, bool visible)
 { String key = F("seriesVisible");
   VFP.set(id, key).arg(seriesIdx).arg(visible).end();
 }

// Set suggested min and max values for the y-axis.
void BarChart::setYRange (float min, float max)
 { String key = F("YRange");
   VFP.set(id, key).arg(min).arg(max).end();
 }

// TODO: When is this called? Can we rely on it?
void BarChart::clearData()
 { VFP.call(id, F("clearData")).end();
   VFP.erase(id);
 }

// Increment a data point in the activeSeries
void BarChart::incrementData(uint8_t catIdx, float amount)
 { if (catIdx < catCount)
    { setData (catIdx, activeSeries[catIdx] + amount);
    }
 }

// Overwrite a data point
void BarChart::setData(uint8_t catIdx, float value)
 { if (catIdx < catCount)
    { VFP.call(id, F("setData")).arg(0).arg(catIdx).arg(value).end();
      activeSeries[catIdx] = value;
    }
 }

// Reset the data for the series seriesIdx to 0.
void BarChart::resetSeries(uint8_t seriesIdx)
 { VFP.call(id, F("clearSeriesData")).arg(seriesIdx).end();
   if (seriesIdx == 0)
    { for (int i = 0; i < catCount; ++i)
       { activeSeries[i] = 0.0f;
       }
    }
 }

// Write series seriesIdx to a named file.
void BarChart::writeSeries(String filename)
 { if (FS(filename))
    { FS.remove(filename);
    }
   Handle fh = FS.open(filename, FILE_WRITE);
   for (int i = 0; i < catCount; ++i)
    { FS.println(fh).arg(String(activeSeries[i])).end();
    }
   FS.close(fh);
 }

// Instruct the js to load the data from file "filename" into the series
// of index seriesIdx (provided that that isn't 0)
void BarChart::loadSeries(uint8_t seriesIdx, String filename)
 { VFP.set(id, F("SeriesDataFile")).arg(seriesIdx).arg(filename).end();
   // TODO: what if seriesIdx == 0? Should we load the activeSeries?
   //       This would require code to read the files.
 }

