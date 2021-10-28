/*
 * EEvar Arduino library
 * author: github.com/AlexIII/EEvar
 * e-mail: endoftheworld@bk.ru
 * license: MIT
 */


#ifndef _EEVAR_H_
#define _EEVAR_H_

#include "EEPROMallocator.h"

template<typename T>
class EEstore : protected EEPROMallocator {
  const void* eeAddr;
public:
  EEstore() : eeAddr(alloc(sizeof(T))) {}
  EEstore(const T& initial) : EEstore() { if(isFirstStart()) *this << initial; }
  const EEstore& operator<<(const T& val) const {
    if(eeAddr) update_block((const uint8_t*)&val, (void*)eeAddr, sizeof(T));
    return *this;
  }
  const EEstore& operator>>(T& val) const {
    if(eeAddr) read_block((uint8_t*)&val, (void*)eeAddr, sizeof(T));
    return *this;
  }
  const T get() const {
    uint8_t buff[sizeof(T)];
    if(eeAddr) read_block(buff, (void*)eeAddr, sizeof(T));
    return *((T*)buff);
  }
};

class EEstring : protected EEPROMallocator {
  const void* eeAddr;
  const uint16_t maxLen;
public:
  EEstring(const uint16_t maxLen, const char* initial = "") : eeAddr(alloc(maxLen)), maxLen(maxLen) { 
    if(isFirstStart()) *this << initial; 
  }
  const EEstring& operator<<(const char* val) const {
    const auto len = strlen(val)+1;
    if(eeAddr) update_block((const uint8_t*)val, (void*)eeAddr, len > maxLen? maxLen : len);
    return *this;
  }   
  const EEstring& operator<<(const String& val) const {
    return *this << val.c_str();
  }
  const EEstring& operator>>(String& val) const {
    char buf[maxLen + 1];
    if(eeAddr) read_block((uint8_t*)buf, (void*)eeAddr, maxLen);
    buf[maxLen] = '\0';
    val = String(buf);
    return *this;
  }
  const String get() const {
    String s;
    *this >> s;
    return s;
  }
};

template<typename T>
class EEvar {
  EEstore<T> store;
  T data;
public:
  EEvar(const T& initial): store(initial) { load(); }
  T& operator*() {return data;}
  const T& operator*() const {return data;}
  T* operator->() {return &data;}
  const T* operator->() const {return &data;}
  void save() const { store << data; }
  void load() { store >> data; }
};


#endif
