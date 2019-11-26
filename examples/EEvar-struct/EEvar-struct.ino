/*
 * EEvar Arduino library
 * author: github.com/AlexIII/EEvar
 * e-mail: endoftheworld@bk.ru
 * license: MIT
 */

/*
 * There's three types available in the library: EEstore<T>, EEstring<MaxLen>, EEvar<T>. 
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
 * EEstring<MaxLen> - use for storing String type. Does not buffer your string. Preserves string length, but no more than MaxLen.
 * Example:
 *    static const EEstring<20> eeVal("hello");  //create and init
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
 *    - All EEstore<T>, EEstring<MaxLen>, EEvar<T> must be global or static.
 *    - Changing order of created EEPROM variables or adding new ones not at the end will corrupt the saved data.
 *    - Type T can only be POD. E.g. T cannot be String and cannot have String as its member. Use EEstring<MaxLen> for storing a String.
 */

#include "EEvar.h"

struct Config {       //structure that we want to store
  bool isOn = false;
  char str[10] = "abc";
};

const EEstore<Config> eeStruct((Config())); //store without buffering

EEvar<Config> configVar((Config())); //store with buffering

void setup() {
  Serial.begin(115200);

  //without buffering
  Config conf;
  eeStruct >> conf;     //load from EEPROM to conf
  Serial.print(conf.isOn);
  Serial.print(" ");
  Serial.println(conf.str);
  conf.isOn = true;     //modify conf
  strncpy(conf.str, "hello", sizeof(conf.str));
  eeStruct << conf;     //save conf to EEPROM

  //with buffering
  Serial.println();
  Serial.print(configVar->isOn);
  Serial.print(" ");
  Serial.println(configVar->str);
  configVar->isOn = true;     //modify configVar
  strncpy(configVar->str, "hello", sizeof(Config::str));
  configVar.save();         //save configVar to EEPROM
  
}

void loop() {
}