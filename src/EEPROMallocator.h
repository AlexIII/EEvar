/*
 * EEvar Arduino library
 * author: github.com/AlexIII/EEvar
 * e-mail: endoftheworld@bk.ru
 * license: MIT
 */

#ifndef _EEPROMALLOCATOR_H_
#define _EEPROMALLOCATOR_H_

#include <EEPROM.h>
#ifndef EE_TEST_VAL
#define EE_TEST_VAL 0x3159
#endif

class EEPROMallocator {
  EEPROMallocator();
  static unsigned int& addrCnt() {
    static unsigned int cur = 2; // Reserve first two bytes for the first start test value
    return cur;
  }
public:
  static void* alloc(const unsigned int sz) {
    unsigned int &cur = addrCnt();
    const unsigned int start = cur;
    cur += sz;
    if(cur > EEPROM.length()) return NULL;
    return (void*)start;
  }

  static unsigned int busy() {
    return addrCnt();
  }
  
  static unsigned int free() {
    return addrCnt() >= EEPROM.length()? 0 : EEPROM.length() - addrCnt();
  }
  
  static bool isFirstStart() {
    static bool checked = false;
    static bool firstStart = true;
    if(checked) return firstStart;
    checked = true;

    uint16_t val;
    if(EEPROM.length() >= sizeof(val)) {
      eeprom_read_block((uint8_t*)&val, (void*)0, sizeof(val));
      firstStart = val != EE_TEST_VAL;
      if(firstStart) {
        val = EE_TEST_VAL;
        eeprom_write_block((uint8_t*)&val, (void*)0, sizeof(val));
      }
    }

    return firstStart;
  }
};

#endif
