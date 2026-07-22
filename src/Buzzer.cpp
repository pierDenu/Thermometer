#include "Buzzer.h"

namespace {
    const unsigned int FREQ_LOW  = 3000;   // Гц — біля резонансу п'єзо (розділ 5 специфікації)
    const unsigned int FREQ_HIGH = 3500;
    const unsigned long WARBLE_PERIOD_MS = 150;   // темп перемикання частот -> ефект "виття", а не рівний тон
}

Buzzer::Buzzer(uint8_t pin_)
    : pin(pin_), warble_timer(WARBLE_PERIOD_MS), active(false), high_tone(false) {}

void Buzzer::alarmOn() {
    if (active) return;   // вже сигналить -> не перезапускати частоту серед фази
    active = true;
    high_tone = false;
    warble_timer.reset();
    tone(pin, FREQ_LOW);
}

void Buzzer::alarmOff() {
    if (!active) return;
    active = false;
    noTone(pin);
}

void Buzzer::update() {
    if (!active) return;
    if (warble_timer.ready()) {
        high_tone = !high_tone;
        tone(pin, high_tone ? FREQ_HIGH : FREQ_LOW);
    }
}
