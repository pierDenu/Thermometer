// ============================================================
//  EepromLimitStore.h
//  (Де)серіалізація меж каналу (FR-3/NFR-4) в/з EEPROM. Єдине місце в
//  проєкті, де щось знає про EEPROM.put/get — ні ChannelTemp, ні
//  сторінки меню цього не торкаються; main.cpp кличе клас у потрібний
//  момент (load() при старті, save() коли LimitSelectPage підняла
//  прапорець save_requested).
// ============================================================
#pragma once

#include <EEPROM.h>
#include "ChannelTemp.h"

class EepromLimitStore {
private:
    // Ознака, що за адресою справді записані межі цією прошивкою — без
    // цього перший запуск (чисте EEPROM = 0xFF) підхопив би сміття
    // замість дефолтних значень з конструктора ChannelTemp.
    static const uint8_t MAGIC = 0xA5;

    struct Record {
        uint8_t magic;
        float low;
        float high;
    };

public:
    // addr — де в EEPROM лежать межі ЦЬОГО каналу; унікальність адрес
    // між каналами — відповідальність викликача (main.cpp).
    static void save(ChannelTemp& channel, int addr) {
        Record rec{ MAGIC, channel.get_low_limit(), channel.get_high_limit() };
        EEPROM.put(addr, rec);   // AVR-ядро само пише лише змінені байти — зайвого зношення flash нема
    }

    static void load(ChannelTemp& channel, int addr) {
        Record rec;
        EEPROM.get(addr, rec);
        if (rec.magic == MAGIC) {
            channel.set_low_limit(rec.low);
            channel.set_high_limit(rec.high);
        }
    }
};
