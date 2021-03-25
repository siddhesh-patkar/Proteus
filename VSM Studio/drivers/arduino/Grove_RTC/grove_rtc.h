#pragma once

#include <ds1307.h>

class GroveRTC : public DS1307
 { public:
      void writeTime(void) { setTime(); }
      void readTime(void)  { getTime(); }  
 };
