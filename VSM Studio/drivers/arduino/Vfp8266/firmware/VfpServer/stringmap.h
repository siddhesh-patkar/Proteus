/********************************************************************/
/********************************************************************/
/*****                                                          *****/
/*****        L A B C E N T E R    E L E C T R O N I C S        *****/
/*****                                                          *****/
/*****                  IoT Builder for Proteus 8               *****/
/*****                                                          *****/
/*****                 ESP8266 StringMap Definition             *****/
/*****                                                          *****/
/********************************************************************/
/********************************************************************/

#include <arduino.h>

class StringMap {
   friend class StringMapIterator;
   public:
      StringMap() { items = NULL; }
      ~StringMap() { clear(); }
      void add (const String &key, const String &value);
      void remove(const String &key);
      void clear();
      String &operator[](const String &key);
      bool contains(const String &key);
      void read (Stream &file);
      void write (Stream &file);
   public:   
      struct Item {
          String key;
          String value;
          Item *next;
      };
      Item *items;  
 };

class StringMapIterator {
    public:
       void operator=(const StringMap &map) { item = map.items; }
       bool operator!=(void *p) { return p != item; }
       void operator++() { item = item->next; }
       const String &key() { return item->key; }
       const String &value() { return item->value; }
    public:
       StringMap::Item *item;          
};


