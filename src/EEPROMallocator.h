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
  static unsigned int& addrCnt() {
    static unsigned int cur = 2; // Reserve first two bytes for the first start test value
    return cur;
  }

public:
  static void* alloc(const unsigned int sz) {
#if defined(ESP8266)
    static bool inited = false;
    if(!inited) EEPROM.begin(512);
    inited = true;
#endif
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
      read_block(&val, (void*)0, sizeof(val));
      firstStart = val != EE_TEST_VAL;
      if(firstStart) {
        val = EE_TEST_VAL;
        update_block(&val, (void*)0, sizeof(val));
      }
    }

    return firstStart;
  }

protected:
  EEPROMallocator() {}

#if defined(__AVR__)

  static void read_block(void *__dst, const void *__src, size_t __n) { eeprom_read_block(__dst, __src, __n); }
  static void update_block(const void *__src, void *__dst, size_t __n) { eeprom_update_block(__src, __dst, __n); }

#elif defined(ESP8266)

  static void read_block(void *__dst, const void *__src, size_t __n) {
    uint8_t* data = (uint8_t*)__dst;
    int addr = (int)__src;
    while(__n--) *data++ = EEPROM[addr++];
  }
  static void update_block(const void *__src, void *__dst, size_t __n) {
    const uint8_t* data = (const uint8_t*)__src;
    int addr = (int)__dst;
    while(__n--) EEPROM.write(addr++, *data++);
    EEPROM.commit();
  }

#endif

};

#endif
