/*
 * PermanentStore.h
 *
 *  Created on: Nov 14, 2016
 *      Author: ghagleitner
 */

#ifndef PERMANENTSTORE_H_
#define PERMANENTSTORE_H_

#include "Radio.h"
#define BUF_SIZE (128)

typedef struct record_ {
  uint8_t crc;
  uint8_t id;
  uint16_t values[4];
} record;

class PermanentStore {
 private:

  // buffer for dummy impl
  uint8_t buf[BUF_SIZE];

  uint8_t crc(const uint8_t *message, int n);
  uint8_t computeId(Model *m);
  uint8_t readByte(int addr);
  void writeByte(int addr, uint8_t v);
  void writeRecord(int addr, record *r);
  void readRecord(int addr, record *r);

  record rec;

 public:

  void save(Model *m);
  void load(Model *m);
};

#endif /* PERMANENTSTORE_H_ */
