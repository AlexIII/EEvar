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
class EEstore {
  const void* eeAddr;
  T m_errVal;
  static const int CHSUM_LEN=1;
  static const int BL_LEN=sizeof(T)+CHSUM_LEN;
  static const int CHSUM_IND=BL_LEN-1;
  uint8_t compCheckSum(const uint8_t * buf){
    uint8_t sum=0;
    for(int i=0;i<sizeof(T);i++)
      sum+=buf[i];
    return sum;  
  }
public:
  EEstore() : eeAddr(EEPROMallocator::alloc(BL_LEN)),m_errVal(0) {}
  EEstore(const T& initial) : EEstore() { if(EEPROMallocator::isFirstStart()) *this << initial; }
  EEstore(const T& initial, const T& errVal) : EEstore(){m_errVal=errVal; if(EEPROMallocator::isFirstStart()) *this << initial; }
  const EEstore& operator<<(const T& val) const {
    uint8_t buf[BL_LEN];
    memcpy(buf,(const uint8_t*)&val,sizeof(T));
    buf[CHSUM_IND]=compCheckSum(buf);
    eeprom_update_block(buf, (void*)eeAddr, BL_LEN);
    return *this;
  }
  const EEstore& operator>>(T& val) const {
    val=get();      
    return *this;
  }
  const T get() const {
    uint8_t buf[BL_LEN];
    eeprom_read_block(buf, (void*)eeAddr, BL_LEN);
    if(buf[CHSUM_IND]==compCheckSum(buf))
      return *((T*)buf);

    return m_errVal;   
  }
};

class EEstring {
  const void* eeAddr;
  const uint16_t maxLen;
  class LString : public String {
  public:  
    bool setSize(unsigned int size) {
      if(!reserve(size)) return false;
      len = size;
      buffer[size] = 0;
      return true;
    }
    void setLen(unsigned int newLen) {
      len = newLen;
    }
  };
public:
  EEstring(const uint16_t maxLen, const char* initial = "") : eeAddr(EEPROMallocator::alloc(maxLen)), maxLen(maxLen) { 
    if(EEPROMallocator::isFirstStart()) *this << initial; 
  }
  const EEstring& operator<<(const char* val) const {
    const auto len = strlen(val)+1;
    eeprom_update_block((const uint8_t*)val, (void*)eeAddr, len > maxLen? maxLen : len);
    return *this;
  }   
  const EEstring& operator<<(const String& val) const {
    return *this << val.c_str();
  }
  const EEstring& operator>>(String& val) const {
    if(!static_cast<LString*>(&val)->setSize(maxLen)) return *this;
    eeprom_read_block((uint8_t*)val.c_str(), (void*)eeAddr, maxLen);
    static_cast<LString*>(&val)->setLen(strlen(val.c_str()));
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
