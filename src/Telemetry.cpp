/*
 * Telemetry.cpp
 *
 *  Created on: Nov 14, 2016
 *      Author: ghagleitner
 */

#include "Telemetry.h"

#define MAX_ITEMS 20

Telemetry::Telemetry()
  : current(0), a1(0), a2(0), rssi(0), hasTelemetry(false)
  //,in(RXPIN,TXPIN,true)
{}

void Telemetry::begin() {
  //pinMode(RXPIN,INPUT);
  //pinMode(TXPIN,OUTPUT);
  //SERIAL_STREAM.begin(9600);
}

uint8_t Telemetry::readByte() {
  return 0;
}

bool Telemetry::available() {
  return false;
}

void Telemetry::update() {
  hasTelemetry = false;
  uint8_t i = 0;

  while ((available() > 0) && (i++ < MAX_ITEMS)) {
    buffer[current] = readByte();

    current = (current+1) % 5;

    if (buffer[current] == 0x7E && buffer[(current+1)%5] == 0xFE) {
      hasTelemetry = true;
      // telemetry header:
      // next bytes: a1 a2 rssi
      a1 = buffer[(current+2)%5];
      a2 = buffer[(current+3)%5];
      rssi = buffer[(current+4)%5];
      break;
    }
  }
}
