#include <math.h>

#include <Wire.h>
#include "Radio.h"
#include "Voltmeter.h"
#include "ModelRegistry.h"
#include "Telemetry.h"
#include "Controller.h"
#include "PermanentStore.h"
#include "Trim.h"
#include "Led.h"

#ifdef ENABLE_ALARM
#include "Alarm.h"
#endif

Voltmeter vmeter1(A1, 0l, 1l);
Voltmeter vmeter2(A2, 0l, 1l);

Telemetry telemetry;

#ifdef ENABLE_ALARM
Alarm alarm(13);
#endif

uint8_t potPins[4] = {A3,A4,A5,A6};
uint8_t switchPins[4] = {2,3,4,5};

Controller controller(potPins, switchPins);

uint8_t trimHighPins[4] = {6,7,8,9};
uint8_t trimLowPins[4] = {10,11,12,14};

Trim trim(trimLowPins,trimHighPins, 13);

uint8_t rbgPins[3] = {A7,A8,A9};
Led led(rbgPins);

#ifdef ENABLE_DUAL_RATES
Radio radio(6, 13);
#else
Radio radio(6);
#endif

ModelRegistry registry;

PermanentStore store;

unsigned long lastM = 0;
unsigned long currentTime = 0;
unsigned int counter = 0;

uint8_t volts[4] = {};
uint8_t signals[1] = {};

void setModel(Model*, int16_t*);
void handleButtons(int16_t *);

#define DELAY 10
#define ITERATIONS(ms) ((ms) / DELAY)

#define THRESHOLD (MAX_LEVEL * 7 / 10)

void setup() {

  // wait for things to stabilize
  delay(500);

#ifdef DEBUG
  Serial.begin(9600);
#endif

  led.begin();
  led.setColor(0,255,0);
  
  radio.begin();

  controller.begin();

  trim.begin();

  registry.begin();
  setModel(registry.current(), controller.getInputs());

  vmeter1.begin();
  vmeter2.begin();

  telemetry.begin();

#ifdef ENABLE_ALARM
  alarm.begin();
#endif
}

void loop() {
  delay(DELAY);
  currentTime = millis();

  if (counter % ITERATIONS(1000) == 0) {
    vmeter1.update();
    vmeter2.update();
    volts[0] = vmeter1.getVoltage();
    volts[1] = vmeter2.getVoltage();

    telemetry.update();

    volts[2] = telemetry.getA1();
    volts[3] = telemetry.getA2();
    signals[0] = telemetry.getRssi();

#ifdef DEBUG
    Serial.println("telemetry: ");
    Serial.println(telemetry.aquired());
    Serial.println(volts[0]);
    Serial.println(volts[1]);
    Serial.println(volts[2]);
    Serial.println(signals[0]);
#endif
  }

#ifdef ENABLE_ALARM
  if (counter % ITERATIONS(300) == 0) {
    alarm.update(currentTime, volts,
                 telemetry.aquired() ? 4 : 2, signals,
                 telemetry.aquired() ? 1 : 0);

    if (alarm.hasSignalAlarm() || alarm.hasVoltAlarm()) {
       led.setColor(255,0,0);
    } else {
       led.setColor(0,255,0);
    }
  }
#endif

  controller.update(currentTime);

  radio.update(controller.getInputs());

  trim.update(currentTime);

  ++counter;
}

void setModel(Model *m, int16_t *inputs) {
  store.load(m);

  radio.setModel(m);

  trim.setTrim(m->trim, m->numInputs);

#ifdef ENABLE_ALARM
  alarm.setMinVolts(m->minVolts);
  alarm.setMinSignals(m->minSignals);
#endif
}
