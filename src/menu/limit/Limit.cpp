#include "Limit.h"
#include "../MenuController.h"
#include <stdlib.h>   // dtostrf

// ---- LimitPageBase --------------------------------------------------

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
    lcd.print(selected && showEditMarker() ? " <" : "  ");
}

float LimitPageBase::valueFor(Field f) const {
    return (f == Field::Low) ? channel.get_low_limit() : channel.get_high_limit();
}

void LimitPageBase::drawLimitRows(LiquidCrystal_I2C& lcd) {
    drawRow(lcd, 0, "Nyzhnya:  ", valueFor(Field::Low),  isFieldSelected(Field::Low));
    drawRow(lcd, 1, "Verkhnya: ", valueFor(Field::High), isFieldSelected(Field::High));
}

void LimitPageBase::drawSaveRow(LiquidCrystal_I2C& lcd) {
    lcd.setCursor(0, 2);
    lcd.print(isSaveSelected() ? "> " : "  ");
    lcd.print("Save");
}

// Кличеться лише по реальних подіях (push/pop/onButton — не за таймером),
// тому діф-кеш тут не потрібен: завжди можна малювати наново.
void LimitPageBase::render(LiquidCrystal_I2C& lcd) {
    drawLimitRows(lcd);
    drawSaveRow(lcd);
}

// ---- LimitValueEditPage ----------------------------------------------

LimitValueEditPage::LimitValueEditPage(ChannelTemp& channel_)
    : LimitPageBase(channel_), editing_value(0) {}

void LimitValueEditPage::beginEditing(Field f) {
    field = f;
    editing_value = (f == Field::Low) ? channel.get_low_limit() : channel.get_high_limit();
}

float LimitValueEditPage::valueFor(Field f) const {
    return (f == field) ? editing_value : LimitPageBase::valueFor(f);
}

void LimitValueEditPage::onButton(MenuButton b, MenuController& nav) {
    // Вгору/Вниз міняють РЕДАГОВАНЕ значення — у channel ще нічого не пишемо,
    // щоб половинчаста зміна не потрапила в об'єкт, якщо вийти без OK.
    if (b == MenuButton::Up)   editing_value += 1.0;
    if (b == MenuButton::Down) editing_value -= 1.0;

    if (b == MenuButton::Ok) {
        // FR-3: нижня межа < верхня межа; некоректне — відхиляємо (лишаємось тут)
        bool valid = (field == Field::Low)
            ? editing_value < channel.get_high_limit()
            : channel.get_low_limit() < editing_value;
        if (valid) {
            if (field == Field::Low) channel.set_low_limit(editing_value);
            else                     channel.set_high_limit(editing_value);
            nav.pop();   // межу збережено -> назад на LimitSelectPage
        }
    }

    if (b == MenuButton::Back) nav.pop();   // вихід без збереження поточного поля
}

// ---- LimitSelectPage --------------------------------------------------

LimitSelectPage::LimitSelectPage(ChannelTemp& channel_, LimitValueEditPage& edit_page_)
    : LimitPageBase(channel_), edit_page(edit_page_), on_save(false) {}

void LimitSelectPage::onEnter() {
    field = Field::Low;   // щоразу при вході курсор на нижній межі
    on_save = false;
}

void LimitSelectPage::onButton(MenuButton b, MenuController& nav) {
    // лише 2 поля -> Up/Down однаково перемикають між ними
    if (b == MenuButton::Up || b == MenuButton::Down) {
        field = (field == Field::Low) ? Field::High : Field::Low;
    }

    if (b == MenuButton::Ok) {
        edit_page.beginEditing(field);
        nav.push(&edit_page);
    }

    if (b == MenuButton::Back) nav.pop();   // вихід зі сторінки цілком
}
