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

StreamList::StreamList ()
 { source = NULL;
   delimiter=0; 
   terminator='\n';
 }
   
StreamList &StreamList::begin(Stream *s) 
 { source=s;  
   return *this; 
 }

StreamList &StreamList::arg (int &i)
// Parse an integer
 { if (source && source->available()) 
    { i = source->parseInt(SKIP_WHITESPACE); 
      skipDelimiter();  
    } 
   else
      i = 0;    
   return *this; 
 }

StreamList &StreamList::arg (long &i)
// Parse an integer
 { if (source && source->available()) 
    { i = source->parseInt(SKIP_WHITESPACE); 
      skipDelimiter();  
    } 
   else
      i = 0;    
   return *this; 
 }
 
StreamList &StreamList::arg (float &f) 
// Parse a float
 { if (source && source->available()) 
    { f = source->parseFloat(SKIP_WHITESPACE); 
      skipDelimiter();  
    } 
   else
      f = 0;    
   return *this; 
 }
    
StreamList &StreamList::arg (String &s)
// Parse a string. If the string contains spaces it must be enclosed in quotes.
 { s = (char *)NULL;
   if (source) 
    { char c = 0;
      while (source->readBytes(&c, 1) == 1 && isSpace(c))
         ;
      if (c == '\"' || c == '\'')
       { // String enclosed in single or double quotes - string is fully escaped.
         s = fromEscaped(c);
         skipDelimiter(); 
       } 
      else
       { // String terminates at whitespace or the delimter.
         s += c;
         while (source->readBytes(&c, 1) == 1 && c != delimiter && !isSpace(c))
            s += c;
         if (c != delimiter && c != '\n')   
            skipDelimiter();
       }
     }
    return *this; 
 }

    
void StreamList::skipDelimiter() 
 { if (source)
    { char c = 0;
      while (c=source->readBytes(&c, 1) != 0 && c != delimiter && c != terminator)
         ;
    }
 }           
 
bool StreamList::isSpace (char c)
// Return true if char is to be considered as a space. 
// This includes newlines and tabs.
 { return c == ' ' ||  c == '\t' || c == '\r' || c == '\n';
 }
 
String StreamList::fromEscaped (char quote)
// Parse and return a C++/Javascript compatible escaped string.
// Complements code in PrintList::toEscaped.
 { String s;
   char c;
   while (source->readBytes(&c, 1) == 1 && c != quote)
    { if (c == '\\')
       { source->readBytes(&c, 1);   
         switch (c)
          { case '0'  : c = 0;    break;
            case '\'' : c = '\''; break;
            case '\"' : c = '\"'; break;
            case '\\' : c = '\\'; break;   
            case 'n'  : c = '\n'; break;
            case 'r'  : c = '\r'; break;
            case 't'  : c = '\t'; break;
            case 'x'  : 
             { char hex[3];
               source->readBytes(hex, 2);
               hex[2] = 0;
               c = strtol(hex, NULL, 16);
               break;
             }
            default  : 
               c = '?'; // Bad escape char
          }
       }
      s += c;      
    }        
   return s;
 }