#pragma once

#include <Arduino.h>
#include "Status.h"

class ChannelTemp {
private: 
  float low_limit;
  float high_limit;
  float curr_temp;
  int thermistor_pin;
  Status state;   // поточний стан тривоги — для гістерезису (FR-4)
public:
  ChannelTemp(int pin, float low, float high);

  void set_low_limit(float new_limit);
  void set_high_limit(float new_limit);
  float get_low_limit();
  float get_high_limit();
  void update();
  Status status();
  float get_temp();
  float measure_temp();
};
