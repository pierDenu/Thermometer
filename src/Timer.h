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
  Timer(unsigned long interval_ms);

  bool ready();  // true, якщо з останнього спрацювання/reset() минув інтервал
  void reset();  // перенести точку відліку на "зараз" (не чекаючи ready())
};
