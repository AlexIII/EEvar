# EEvar - EEPROM Arduino library

Simple and lightweight library for Arduino that allows you to save your variables in EEPROM memory. 

No need to keep track of addresses, offsets, and sizes of data that you want to store in EEPROM. After saving your variable to EEPROM its value gets restored after power failure or CPU reset.

Works with any POD (`bool`, `int`, `float`, custom structs, etc.) and `String`. 

## Usage

### Example

#### Simple types and `String`

```c++
#include "EEvar.h"

const EEstore<float> eeFloat(25.8);       //allocate EEPROM for storing float and save value 25.8 to it on the first start

const EEstore<int> eeInt(-3685);          //for storing int

const EEstring<20> eeString("initial");   //for storing String (20 chars max)

void setup() {
  Serial.begin(115200);

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

```

#### Structures

```c++
#include "EEvar.h"

struct Config {				//structure that we want to store
  bool isOn = false;
  char str[10] = "abc";
};

const EEstore<Config> eeStruct((Config())); //store without buffering

EEvar<Config> configVar((Config())); //store with buffering

void setup() {
  Serial.begin(115200);

  //without buffering
  Config conf;
  eeStruct >> conf;			//load from EEPROM to conf
  Serial.print(conf.isOn);
  Serial.print(" ");
  Serial.println(conf.str);
  conf.isOn = true;			//modify conf
  strncpy(conf.str, "hello", sizeof(v3.str));
  eeStruct << conf;			//save conf to EEPROM

  //with buffering
  Serial.println();
  Serial.print(configVar->isOn);
  Serial.print(" ");
  Serial.println(configVar->str);
  configVar->isOn = true;			//modify configVar
  strncpy(configVar->str, "hello", sizeof(Config::str));
  configVar.save();					//save configVar to EEPROM
  
}

void loop() {
}

```



### Important notes

- All `EEstore<T>`, `EEstring<MaxLen>`, `EEvar<T>` must be global or static (or in another way ensure a stable order of instantiations).
- Changing order of created EEPROM variables or adding new ones not at the end will corrupt the saved data.
- Type `T` can only be POD (`bool`, `int`, `float`, custom structs, etc.). `T` cannot be `String` and cannot have `String` as its member. Use `EEstring<MaxLen>` for storing a `String`.



### API

There's three types available in the library: 

- `EEstore<T>` - does not create buffer of your type `T`. Use for:
  - simple types like `bool`, `int`, `float`;
  - large structures, that you don't need to have in the RAM all the time.
-  `EEvar<T>` - creates buffer of your type `T`. Use for:
  - values that you read frequently;
  - complex structures.
-  `EEstring<MaxLen>` - use for storing `String` type. Does not buffer your string. Preserves string length, but no more than MaxLen.

Comparison table:

|                                     | `EEstore<T>` | `EEvar<T>`    | `EEstring<MaxLen>`         |
| ----------------------------------- | ------------ | ------------- | -------------------------- |
| Can store                           | POD          | POD           | String of length <= MaxLen |
| Buffered                            | no           | yes           | no                         |
| Easy struct-field access (via `->`) | no           | yes           | -                          |
| Size, bytes                         | 2            | 2 + sizeof(T) | 2                          |



All available classes and methods:

```c++
class EEPROMallocator {
  static void* alloc(const uint16_t sz);    //allocate EEPROM bytes (used by the library, don't call it directly)
  static uint16_t busy();                   //counter of busy EEPROM, bytes
  static uint16_t free();                   //counter of free EEPROM, bytes
  static bool isFirstStart();               //check if it's CPU's first start
};

template<typename T>
class EEstore {
  EEstore();
  EEstore(const T& initial);
  const EEstore& operator<<(const T& val) const;  //save val to EEPROM
  const EEstore& operator>>(T& val) const;        //load to val from EEPROM
  const T get() const;                            //read from EEPROM
};

template<unsigned int MaxLen>
class EEstring {
  EEstring();
  EEstring(const char* initial);
  const EEstring& operator<<(const char* val) const;      //save char string to EEPROM
  const EEstring& operator<<(const String& val) const;    //save String to EEPROM
  const EEstring& operator>>(String& val) const;          //load to String val from EEPROM
  const String get() const;                               //read from EEPROM
};

template<typename T>
class EEvar {
  EEvar(const T& initial);
  T& operator*();               //access struct
  const T& operator*();         //access struct
  T* operator->();              //access struct
  const T* operator->() const;  //access struct
  void save() const;            //save to EEPROM
  void load();                  //load from EEPROM
};
```



## Architecture

AVR-based Arduino: Uno, Nano, Mini Pro, 2560, etc.

## License

MIT