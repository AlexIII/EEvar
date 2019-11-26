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
  static uint16_t& addrCnt() {
    static uint16_t cur = 0;
    return cur;
  }
public:
  static void* alloc(const uint16_t sz) {
    uint16_t &cur = addrCnt();
    const uint16_t t = cur;
    cur += sz;
    return (void*)t;
  }

  static uint16_t busy() {
    return addrCnt();
  }
  
  static uint16_t free() {
    return addrCnt() >= EEPROM.length()? 0 : EEPROM.length() - addrCnt();
  }
  
  static bool isFirstStart() {
    static bool checked = false;
    static bool result = false;
    if(checked) return result;

    void* addr = alloc(2);
    uint16_t val;
    eeprom_read_block((uint8_t*)&val, addr, sizeof(val));
    if(val != EE_TEST_VAL) {
      val = EE_TEST_VAL;
      eeprom_write_block((uint8_t*)&val, addr, sizeof(val));
      result = true;
    }

    checked = true;
    return result;
  }
};

#endif
