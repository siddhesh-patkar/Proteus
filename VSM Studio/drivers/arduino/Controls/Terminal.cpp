#include "core.h"
#include "vfpserver.h"
#include "controls.h"

Terminal::Terminal (const char *_id) : Control(_id) {  }
     
void Terminal::cls() 
 {  VFP.call(id, F("cls")).end(); 
    VFP.erase(id);  
 }      

PrintList &Terminal::print() { return begin(false, &text); }
PrintList &Terminal::println() { return begin(true, &text); }
      
void Terminal::end() 
 { PrintList::end(); 
   VFP.record(id, true);
   VFP.call(id, F("appendText")).arg(text.buffer()).end(); 
   text.clear(); 
 }
                
