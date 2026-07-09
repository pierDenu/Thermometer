#include "Timer.h"

Timer::Timer(unsigned long interval)
    : interval_ms(interval),
      last_time(millis())
{
}

bool Timer::ready() {
    unsigned long now = millis();
    // now - last_time коректно рахує навіть після переповнення millis()
    // (арифметика unsigned long зациклюється так само, як і сам millis()).
    if (now - last_time >= interval_ms) {
        last_time = now;   // відлік наступного інтервалу — від "зараз", без накопичення боргу
        return true;
    }
    return false;
}

void Timer::reset() {
    last_time = millis();
}
