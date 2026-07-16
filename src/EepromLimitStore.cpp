#include "EepromLimitStore.h"
#include <EEPROM.h>

namespace {
    // Ознака, що за адресою справді записані межі цією прошивкою — без
    // цього перший запуск (чисте EEPROM = 0xFF) підхопив би сміття
    // замість дефолтних значень з конструктора ChannelTemp.
    const uint8_t MAGIC = 0xA5;

    struct Record {
        uint8_t magic;
        float low;
        float high;
    };
}

void EepromLimitStore::save(ChannelTemp& channel, int addr) {
    Record rec{ MAGIC, channel.get_low_limit(), channel.get_high_limit() };
    EEPROM.put(addr, rec);   // AVR-ядро само пише лише змінені байти — зайвого зношення flash нема
}

void EepromLimitStore::load(ChannelTemp& channel, int addr) {
    Record rec;
    EEPROM.get(addr, rec);
    if (rec.magic == MAGIC) {
        channel.set_low_limit(rec.low);
        channel.set_high_limit(rec.high);
    }
}
