// ============================================================
//  Buzzer.h — пасивний п'єзо-бузер (FR-4). Веде себе як просте
//  апаратне джерело "виття": alarmOn()/alarmOff() кажуть, чи має
//  зараз лунати сигнал, а update() (кличеться щоцикл) сама
//  перемикає частоти, поки увімкнено. Нічого не знає про канали
//  чи acknowledge — це турбота ChannelTemp/main.cpp.
// ============================================================
#pragma once

#include <Arduino.h>
#include "Timer.h"

class Buzzer {
private:
    uint8_t pin;
    Timer warble_timer;
    bool active;
    bool high_tone;   // яка з двох частот зараз лунає

public:
    explicit Buzzer(uint8_t pin_);

    void alarmOn();    // почати сигнал; якщо вже увімкнено — нічого не робить
    void alarmOff();   // зупинити сигнал
    void update();      // викликати щоцикл — перемикає частоти для ефекту "виття"
};
