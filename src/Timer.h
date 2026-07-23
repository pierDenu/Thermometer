// ============================================================
//  Timer.h
//  Небло­куючий таймер: об'єкт лише пам'ятає інтервал і точку
//  відліку. Сам нічого не запускає — у loop() питаєш ready(),
//  і якщо true, подію викликаєш сам, у відповідь на результат.
// ============================================================
#pragma once

#include <Arduino.h>

class Timer {
private:
  unsigned long interval_ms;
  unsigned long last_time;
public:
  Timer(unsigned long interval_ms) : interval_ms(interval_ms), last_time(millis()) {}

  // true, якщо з останнього спрацювання/reset() минув інтервал
  bool ready() {
    unsigned long now = millis();
    // now - last_time коректно рахує навіть після переповнення millis()
    // (арифметика unsigned long зациклюється так само, як і сам millis()).
    if (now - last_time >= interval_ms) {
      last_time = now;   // відлік наступного інтервалу — від "зараз", без накопичення боргу
      return true;
    }
    return false;
  }

  void reset() { last_time = millis(); }   // перенести точку відліку на "зараз" (не чекаючи ready())
};
