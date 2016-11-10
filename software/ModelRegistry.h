#ifndef ModelRegistry_h
#define ModelRegistry_h

#include "Arduino.h"
#include "Model.h"
#include "Battery.h"

#define NUM_MODELS 1

// reta
class ModelRegistry {
 private:

  int8_t one[8][8] = {
    {
      PERCENT_TO_BYTE(100),
      PERCENT_TO_BYTE(0),
      PERCENT_TO_BYTE(0),
      PERCENT_TO_BYTE(0),
      PERCENT_TO_BYTE(0),
      PERCENT_TO_BYTE(0),
      PERCENT_TO_BYTE(0),
      PERCENT_TO_BYTE(0),
    },
    {
      PERCENT_TO_BYTE(0),
      PERCENT_TO_BYTE(100),
      PERCENT_TO_BYTE(0),
      PERCENT_TO_BYTE(0),
      PERCENT_TO_BYTE(0),
      PERCENT_TO_BYTE(0),
      PERCENT_TO_BYTE(0),
      PERCENT_TO_BYTE(0),

    },
    {
      PERCENT_TO_BYTE(0),
      PERCENT_TO_BYTE(0),
      PERCENT_TO_BYTE(100),
      PERCENT_TO_BYTE(0),
      PERCENT_TO_BYTE(0),
      PERCENT_TO_BYTE(0),
      PERCENT_TO_BYTE(0),
      PERCENT_TO_BYTE(0)
    },
    {
      PERCENT_TO_BYTE(0),
      PERCENT_TO_BYTE(0),
      PERCENT_TO_BYTE(0),
      PERCENT_TO_BYTE(100),
      PERCENT_TO_BYTE(0),
      PERCENT_TO_BYTE(0),
      PERCENT_TO_BYTE(0),
      PERCENT_TO_BYTE(0)
    },
    {
      PERCENT_TO_BYTE(0),
      PERCENT_TO_BYTE(0),
      PERCENT_TO_BYTE(0),
      PERCENT_TO_BYTE(0),
      PERCENT_TO_BYTE(100),
      PERCENT_TO_BYTE(0),
      PERCENT_TO_BYTE(0),
      PERCENT_TO_BYTE(0),
    },
    {
      PERCENT_TO_BYTE(0),
      PERCENT_TO_BYTE(0),
      PERCENT_TO_BYTE(0),
      PERCENT_TO_BYTE(0),
      PERCENT_TO_BYTE(0),
      PERCENT_TO_BYTE(100),
      PERCENT_TO_BYTE(0),
      PERCENT_TO_BYTE(0),

    },
    {
      PERCENT_TO_BYTE(0),
      PERCENT_TO_BYTE(0),
      PERCENT_TO_BYTE(0),
      PERCENT_TO_BYTE(0),
      PERCENT_TO_BYTE(0),
      PERCENT_TO_BYTE(0),
      PERCENT_TO_BYTE(100),
      PERCENT_TO_BYTE(0)
    },
    {
      PERCENT_TO_BYTE(0),
      PERCENT_TO_BYTE(0),
      PERCENT_TO_BYTE(0),
      PERCENT_TO_BYTE(0),
      PERCENT_TO_BYTE(0),
      PERCENT_TO_BYTE(0),
      PERCENT_TO_BYTE(0),
      PERCENT_TO_BYTE(100)
    }
  };

  int16_t input[8] = {};
  int16_t channels[8] = {};
  int16_t trim[8] = {};

  int8_t expo[8] = {
    PERCENT_TO_BYTE(50),
    PERCENT_TO_BYTE(50),
    PERCENT_TO_BYTE(50),
    PERCENT_TO_BYTE(50),
    PERCENT_TO_BYTE(0),
    PERCENT_TO_BYTE(0),
    PERCENT_TO_BYTE(0),
    PERCENT_TO_BYTE(0)
  };

#ifdef ENABLE_DUAL_RATES
  int8_t drLow[8] = {
    PERCENT_TO_BYTE(80),
    PERCENT_TO_BYTE(80),
    PERCENT_TO_BYTE(80),
    PERCENT_TO_BYTE(80),
    PERCENT_TO_BYTE(80),
    PERCENT_TO_BYTE(80),
    PERCENT_TO_BYTE(80),
    PERCENT_TO_BYTE(80)
  };

  int8_t drHigh[8] = {
    PERCENT_TO_BYTE(100),
    PERCENT_TO_BYTE(100),
    PERCENT_TO_BYTE(100),
    PERCENT_TO_BYTE(100),
    PERCENT_TO_BYTE(100),
    PERCENT_TO_BYTE(100),
    PERCENT_TO_BYTE(100),
    PERCENT_TO_BYTE(100)
  };
#endif

#ifdef ENABLE_ALARM
  uint8_t minVolts[4] = {VOLTS_TO_BYTE(WARN_VOLTS_LIPO(1)),
                         VOLTS_TO_BYTE(WARN_VOLTS_LIPO(1)),
                         VOLTS_TO_BYTE(WARN_VOLTS_LIPO(4)),
                         0xFF};

  uint8_t minSignals[1] = {20};
#endif

  uint8_t current_;

  Model models[NUM_MODELS] =
    {
      {
        "quad",
        expo,
#ifdef ENABLE_DUAL_RATES
        drLow,
        drHigh,
#endif
#ifdef ENABLE_ALARM
        minVolts,
        minSignals,
#endif
        (int8_t*)one,
        channels,
        input,
        trim,
        8,
        8,
        0
      }
    };

 public:

  void begin();
  Model *current();
  Model *next();
  Model *previous();
};

#endif
