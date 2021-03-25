/********************************************************************/
/********************************************************************/
/*****                                                          *****/
/*****        L A B C E N T E R    E L E C T R O N I C S        *****/
/*****                                                          *****/
/*****                 Visual Designer for Arduino              *****/
/*****                                                          *****/
/*****            Stream List (Wrap for Class Stream)           *****/
/*****                                                          *****/
/********************************************************************/
/********************************************************************/

#include <arduino.h>
#include <ctype.h>

#include "core.h"

PrintList::PrintList () 
 { target = NULL;
   newLine=false;
   argCount = 0;
   delimiter = 0;
   terminator = 0;
   places = 2;
   base = DEC; 
 }  
 
PrintList &PrintList::begin (bool eol, Print *p) 
 { target=p; 
   newLine = eol; 
   argCount = 0;
   return *this; 
 }

PrintList &PrintList::arg(int i) 
 { if (target) 
    { nextItem(); 
      target->print(i,base); 
    } 
   return *this; 
 }

PrintList &PrintList::arg(long i) 
 { if (target) 
    { nextItem(); 
      target->print(i,base); 
    } 
   return *this; 
 }

PrintList &PrintList::arg(double f) 
 { if (target) 
    { nextItem(); 
      target->print(f,places); 
    } 
   return *this; 
 }

PrintList &PrintList::arg(const char *s) 
 { if (target) 
    { nextItem(); 
      if (delimiter != 0)
         toEscaped(s);
      else     
         target->print(s); 
     } 
    return *this; 
 }

PrintList &PrintList::arg(const String &s) 
 { if (target) 
    { nextItem(); 
      if (delimiter != 0)
         toEscaped(s); 
      else     
         target->print(s); 
    } 
   return *this; 
 }

PrintList &PrintList::arg(const __FlashStringHelper *s) 
 { if (target) 
    { nextItem(); 
      if (delimiter != 0)
       { target->print('\"');
         target->print(s); 
         target->print('\"');
       }
      else    
         target->print(s); 
    } 
   return *this; 
 }

void PrintList::nextItem()
 { if (delimiter != 0)
       if (argCount++ > 0)
          target->print(delimiter);
 }    

void PrintList::toEscaped (const String &s)
// Print a string with C++/Javascript compatible escape characters.
// Complements the code in StreamList::unEscape.
 { target->print('\"');
   for (int i=0; i<s.length(); ++i)
    { byte c = s[i];
      if (c >= 32 && c<256 && c != '\'' && c != '\"')
         target->print(s[i]);
      else   
       { target->print('\\');
         switch (c)
          { case '0'  : target->print('0'); break;
            case '\'' : target->print('\''); break;
            case '\"' : target->print('\"'); break;
            case '\\' : target->print('\\'); break;   
            case '\n' : target->print('n'); break;   
            case '\r' : target->print('r'); break;   
            case '\t' : target->print('t'); break;               
            default   :  // Print as xHH (two hex digits)
               target->print('x'); 
               target->print(c >> 8, HEX); 
               target->print(c & 0x0F , HEX);
          }
       }
    }
   target->print('\"');    
 }
 
void PrintList::end() 
 { if (target)
    { if (terminator)
         target->print(terminator);         
      if (newLine) 
         target->println();  
    }
      
 }
