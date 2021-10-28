/*
 * EEvar Arduino library
 * author: github.com/AlexIII/EEvar
 * e-mail: endoftheworld@bk.ru
 * license: MIT
 */

/*
 * There's three types available in the library: EEstore<T>, EEstring, EEvar<T>. 
 * What to choose:
 * 
 * EEstore<T> - does not create buffer of your type T. Use for:
 *    - simple types like bool, int, float.
 *    - for storing large structures, that you don't need to have in the RAM all the time, only when you access it.
 * Example:
 *    static const EEstore<int> eeVal(5);  //create and init
 *    int a = eeVal.get();          //read
 *    int b;
 *    eeVal >> b;                   //read
 *    b = -18;
 *    eeVal << b;                   //write
 *    
 * EEstring - use for storing String type. Does not buffer your string. 
 * Preserves string length, but no more than maxLen (first constructor argument).
 * Example:
 *    static const EEstring eeVal(20, "hello");  //create and init. Max 20 chars.
 *    String a = eeVal.get();       //read
 *    String b;
 *    eeVal >> b;                   //read
 *    eeVal << "world";             //write 
 * 
 * EEvar<T> - creates buffer of your type T. Use for:
 *    - values that you read frequently.
 *    - complex structures.
 * Example:
 *    struct Config {
 *      int a = 5;
 *      bool b = false;
 *      folat c = 50.6;
 *    };
 *    static EEvar<Config> config((Config()));
 *    int a = config->a;      //read a
 *    config->b = true;       //modify b in config (but not save it)
 *    config.load();          //reverse changes in config
 *    config->c = 80;         //modify c in config (but not save it)
 *    config.save();          //save changes in config
 *    
 * Important notes: 
 *    - All EEstore<T>, EEstring, EEvar<T> must be global or static.
 *    - Changing order of created EEPROM variables or adding new ones not at the end will corrupt the saved data.
 *    - Type T can only be POD. E.g. T cannot be String and cannot have String as its member. Use EEstring for storing a String.
 */


#include "EEvar.h"

const EEstore<float> eeFloat(25.8);       //allocate EEPROM for storing float and save value 25.8 to it on the first start

const EEstore<int> eeInt(-3685);          //for storing int

const EEstring eeString(20, "initial");   //for storing String (20 chars max)

void setup() {
  Serial.begin(115200);
  Serial.println();

  //check if it's CPU first start
  Serial.println(
      EEPROMallocator::isFirstStart()
          ? F("First start") 
          : F("Not first start")
  );

  //float
  float v1;
  eeFloat >> v1;      //load from eeprom to v1
  Serial.println(v1);
  //Serial.println(eeFloat.get()); //same as above
  v1 = 99.2;
  eeFloat << v1;      //save v1 to eeprom

  //int
  Serial.println(eeInt.get());
  eeInt << 56;

  //String
  String v4;
  eeString >> v4;
  Serial.println(v4);
  v4 = "long string, not gonna fit";
  eeString << v4;
  
}

void loop() {
}
