#include "LimitPageBase.h"
#include <stdlib.h>   // dtostrf

LimitPageBase::LimitPageBase(ChannelTemp& channel_)
    : channel(channel_), field(Field::Low) {}

void LimitPageBase::printField(LiquidCrystal_I2C& lcd, float v) {
    char buf[7];
    dtostrf(v, 5, 1, buf);
    lcd.print(buf);
}

void LimitPageBase::drawRow(LiquidCrystal_I2C& lcd, int row, const char* label, float v, bool selected) {
    lcd.setCursor(0, row);
    lcd.print(selected ? "> " : "  ");
    lcd.print(label);
    printField(lcd, v);
    lcd.print(selected ? " <" : "  ");
}

// render() кличеться лише по реальних подіях (push/pop/onButton — не за
// таймером), тому діф-кеш тут не потрібен: завжди можна малювати наново.
void LimitPageBase::render(LiquidCrystal_I2C& lcd) {
    drawAll(lcd);
}
