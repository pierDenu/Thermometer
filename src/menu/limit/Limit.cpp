#include "Limit.h"
#include "../MenuController.h"
#include <stdlib.h>   // dtostrf

namespace {
    const int LABEL_COL     = 2;    // 0-1 лишаються під "> " з drawCursor()
    const int LABEL_WIDTH   = 10;   // ширина "Nyzhnya:  " / "Verkhnya: " (вирівняні однаково)
    const int VALUE_WIDTH   = 5;    // printField() завжди друкує 5 символів (dtostrf(v,5,1,...))
    const int VALUE_END_COL = LABEL_COL + LABEL_WIDTH + VALUE_WIDTH;   // де починається " <"
}

// ---- LimitPageBase --------------------------------------------------

LimitPageBase::LimitPageBase(ChannelTemp& channel_)
    : channel(channel_), field(Field::Low), cursor(0) {}

void LimitPageBase::printField(LiquidCrystal_I2C& lcd, float v) {
    char buf[7];
    dtostrf(v, 5, 1, buf);
    lcd.print(buf);
}

void LimitPageBase::drawRow(LiquidCrystal_I2C& lcd, int row, const char* label, float v) {
    lcd.setCursor(LABEL_COL, row);
    lcd.print(label);
    printField(lcd, v);
}

float LimitPageBase::valueFor(Field f) const {
    return (f == Field::Low) ? channel.get_low_limit() : channel.get_high_limit();
}

void LimitPageBase::drawCursor(LiquidCrystal_I2C& lcd) {
    for (int row = 0; row < 3; row++) {   // очищаємо "> " на всіх рядках -> без привида на старій позиції курсора
        lcd.setCursor(0, row);
        lcd.print(row == cursor ? "> " : "  ");
    }
}

void LimitPageBase::drawEditMarker(LiquidCrystal_I2C& lcd) {
    lcd.setCursor(VALUE_END_COL, cursor);
    lcd.print(" <");
}

void LimitPageBase::drawLimitRows(LiquidCrystal_I2C& lcd) {
    drawRow(lcd, 0, "Nyzhnya:  ", valueFor(Field::Low));
    drawRow(lcd, 1, "Verkhnya: ", valueFor(Field::High));
}

void LimitPageBase::drawSaveRow(LiquidCrystal_I2C& lcd) {
    lcd.setCursor(LABEL_COL, 2);
    lcd.print("Save");
}

void LimitPageBase::render(LiquidCrystal_I2C& lcd) {
    drawLimitRows(lcd);
    drawSaveRow(lcd);
    drawCursor(lcd);
    if (showEditMarker()) drawEditMarker(lcd);
}

// ---- LimitValueEditPage ----------------------------------------------

LimitValueEditPage::LimitValueEditPage(ChannelTemp& channel_)
    : LimitPageBase(channel_), editing_value(0) {}

void LimitValueEditPage::beginEditing(Field f) {
    field = f;
    cursor = (f == Field::Low) ? 0 : 1;   // курсор/маркер на тому ж рядку, що й обране поле
    editing_value = (f == Field::Low) ? channel.get_low_limit() : channel.get_high_limit();
}

float LimitValueEditPage::valueFor(Field f) const {
    return (f == field) ? editing_value : LimitPageBase::valueFor(f);
}

void LimitValueEditPage::onButton(MenuButton b, MenuController& nav) {
    if (b == MenuButton::Up)   editing_value += 1.0;   // міняє лише editing_value — в channel ще нічого не пишемо
    if (b == MenuButton::Down) editing_value -= 1.0;

    if (b == MenuButton::Ok) {
        bool valid = (field == Field::Low)   // FR-3: нижня межа < верхня; некоректне — відхиляємо, лишаємось тут
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

LimitPageBase::Field LimitSelectPage::fieldForCursor(int c) {
    switch (c) {
        case 0:  return Field::Low;
        case 1:  return Field::High;
        default: return Field::Save;
    }
}

LimitSelectPage::LimitSelectPage(ChannelTemp& channel_, LimitValueEditPage& edit_page_)
    : LimitPageBase(channel_), edit_page(edit_page_), save_requested(false) {}

void LimitSelectPage::onEnter() {
    cursor = 0;
    field = Field::Low;   // щоразу при вході курсор на нижній межі
}

void LimitSelectPage::onButton(MenuButton b, MenuController& nav) {
    const int CURSOR_COUNT = 3;   // Low, High, Save
    if (b == MenuButton::Down) cursor = (cursor + 1) % CURSOR_COUNT;
    if (b == MenuButton::Up)   cursor = (cursor - 1 + CURSOR_COUNT) % CURSOR_COUNT;
    field = fieldForCursor(cursor);   // field лишається тим, що читає спільний render() базового класу

    if (b == MenuButton::Ok) {
        if (field == Field::Save) {
            save_requested = true;   // main.cpp побачить це щоцикл і сам запише в EEPROM
            nav.pop();                // збереження підтверджено -> назад на ChannelListPage
        } else {
            edit_page.beginEditing(field);   // тут field гарантовано Low або High
            nav.push(&edit_page);
        }
    }

    if (b == MenuButton::Back) nav.pop();   // вихід зі сторінки цілком
}
