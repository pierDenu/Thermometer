#pragma once

#include <Arduino.h>
#include "Status.h"

class ChannelTemp {
private: 
  float low_limit;
  float high_limit;
  float curr_temp;
  int thermistor_pin;
  float last_saved_low;   // межа на момент останнього mark_saved() — для позначки "не збережено"
  float last_saved_high;  // межа на момент останнього mark_saved() — для позначки "не збережено"
  Status state;   // поточний стан тривоги — для гістерезису (FR-4)
  bool sound_acknowledged;   // true -> звук цієї тривоги вже заглушено кнопкою "Mute" (FR-4)
public:
  ChannelTemp(int pin, float low, float high);

  void set_low_limit(float new_limit);
  void set_high_limit(float new_limit);
  float get_low_limit();
  float get_high_limit();
  bool is_low_dirty() const { return low_limit != last_saved_low; }
  bool is_high_dirty() const { return high_limit != last_saved_high; }
  void mark_saved() { last_saved_low = low_limit; last_saved_high = high_limit; }
  bool needs_sound() const { return state != Status::Ok && !sound_acknowledged; }
  void acknowledge_sound() { sound_acknowledged = true; }
  void update();
  Status status();
  float get_temp();
  float measure_temp();
};
