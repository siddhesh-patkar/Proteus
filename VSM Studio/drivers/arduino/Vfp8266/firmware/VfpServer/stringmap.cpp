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

#include "StringMap.h"

String &StringMap::operator[] (const String &key) {
   Item *item = items;
   while (item != NULL && item->key != key)
      item = item->next;
   if (item == NULL) {
      item = new Item;
      item->key = key;
      item->next = items;
      items = item;
   }      
   return item->value;
}

bool StringMap::contains(const String &key) {
   Item *item = items;
   while (item != NULL && item->key != key)
      item = item->next;
   return item != NULL;     
}

void StringMap::add (const String &key, const String &value) {
   Item *item = items;
   while (item != NULL && item->key != key)
      item = item->next;
   if (item == NULL) {
      item = new Item;
      item->key = key;
      item->next = items;
      items = item;
   }      
   item->value = value;
}

void StringMap::remove(const String &key) {
   Item *item = items, *prev = NULL;
   while (item != NULL && item->key != key)
       prev = item, item = item->next;
   if (item != NULL) {
      if (prev != NULL)
         prev->next = item->next;
      else
         items = item->next;
      delete item;    
   }      
}

void StringMap::clear() {
   while (items != NULL) {
      Item *item = items;
      items=items->next;
      delete item;
   }
}
 
void StringMap::read (Stream &file) {
   while (file.available() > 0) {
      String line = file.readStringUntil('\n');
      //Serial.println(line);
      int eq = line.indexOf('=');
      if (eq != -1) {
         line.replace('\r', '\0');
         add(line.substring(0, eq), line.substring(eq+1));
      }
   }    
}
    
void StringMap::write (Stream &file) {
   Item *item = items;
   while (item != NULL) {
       file.print(item->key);
       file.print('=');
       file.println(item->value);    
       item = item->next;
   }
}  

