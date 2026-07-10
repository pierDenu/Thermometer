#include "LimitValueEditPage.h"
#include "MenuController.h"

LimitValueEditPage::LimitValueEditPage(ChannelTemp& channel_)
    : LimitPageBase(channel_), editing_value(0) {}

void LimitValueEditPage::beginEditing(Field f) {
    field = f;
    editing_value = (f == Field::Low) ? channel.get_low_limit() : channel.get_high_limit();
}

void LimitValueEditPage::drawAll(LiquidCrystal_I2C& lcd) {
    drawRow(lcd, 0, "Nyzhnya:  ", field == Field::Low  ? editing_value : channel.get_low_limit(),  field == Field::Low);
    drawRow(lcd, 1, "Verkhnya: ", field == Field::High ? editing_value : channel.get_high_limit(), field == Field::High);
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
