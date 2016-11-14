/*
 * Telemetry.h
 *
 *  Created on: Nov 14, 2016
 *      Author: ghagleitner
 */

#ifndef TELEMETRY_H_
#define TELEMETRY_H_

#include <stdint.h>
#include "Battery.h"

#define RXPIN 0
#define TXPIN 1
#define SERIAL_STREAM in

class Telemetry {

 private:
  uint8_t current;
  uint8_t a1;
  uint8_t a2;
  uint8_t rssi;
  bool hasTelemetry;
  uint8_t buffer[5] = {};
  //SoftwareSerial in;

  uint8_t readByte();
  bool available();

 public:

  Telemetry();

  void begin();
  void update();
  inline uint8_t getRssi();
  inline uint8_t getA1();
  inline uint8_t getA2();
  inline bool aquired();
};

inline uint8_t Telemetry::getRssi() {
  return rssi;
}

inline uint8_t Telemetry::getA1() {
  return A1_TO_BYTE(a1);
}

inline uint8_t Telemetry::getA2() {
  return A2_TO_BYTE(a2);
}

inline bool Telemetry::aquired() {
  return hasTelemetry;
}

#endif /* TELEMETRY_H_ */
