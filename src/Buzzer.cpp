#include "Buzzer.h"

namespace {
    const unsigned long BEEP_PERIOD_MS = 200;   // тривалість фази "біп" і фази паузи
}

Buzzer::Buzzer(uint8_t pin_)
    : pin(pin_), beep_timer(BEEP_PERIOD_MS), active(false), sounding(false) {
    pinMode(pin, OUTPUT);
    digitalWrite(pin, LOW);
}

void Buzzer::alarmOn() {
    if (active) return;   // вже сигналить -> не перезапускати фазу серед біпу
    active = true;
    sounding = true;
    beep_timer.reset();
    digitalWrite(pin, HIGH);
}

void Buzzer::alarmOff() {
    if (!active) return;
    active = false;
    digitalWrite(pin, LOW);
}

void Buzzer::update() {
    if (!active) return;
    if (beep_timer.ready()) {
        sounding = !sounding;
        digitalWrite(pin, sounding ? HIGH : LOW);
    }
}
