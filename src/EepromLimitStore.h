// ============================================================
//  EepromLimitStore.h
//  (Де)серіалізація меж каналу (FR-3/NFR-4) в/з EEPROM. Єдине місце в
//  проєкті, де щось знає про EEPROM.put/get — ні ChannelTemp, ні
//  сторінки меню цього не торкаються; main.cpp кличе клас у потрібний
//  момент (load() при старті, save() коли LimitSelectPage підняла
//  прапорець save_requested).
// ============================================================
#pragma once

#include "ChannelTemp.h"

class EepromLimitStore {
public:
    // addr — де в EEPROM лежать межі ЦЬОГО каналу; унікальність адрес
    // між каналами — відповідальність викликача (main.cpp).
    static void save(ChannelTemp& channel, int addr);
    static void load(ChannelTemp& channel, int addr);
};
