#include "LimitEditPage.h"
#include "MenuController.h"

LimitEditPage::LimitEditPage(ChannelTemp& channel_)
    : channel(channel_), field(Field::Low), editing_value(0) {}

void LimitEditPage::onEnter() {
    // щоразу при вході починаємо з нижньої межі, підвантаживши поточне значення
    field = Field::Low;
    editing_value = channel.get_low_limit();
}

void LimitEditPage::render(LiquidCrystal_I2C& lcd) {
    lcd.setCursor(0, 0);
    lcd.print(field == Field::Low ? "Nyzhnya mezha:" : "Verkhnya mezha:");
    lcd.setCursor(0, 1);
    lcd.print(editing_value, 1);
}

void LimitEditPage::onButton(MenuButton b, MenuController& nav) {
    // Вгору/Вниз міняють РЕДАГОВАНЕ значення — у channel ще нічого не пишемо,
    // щоб половинчаста зміна не потрапила в об'єкт, якщо вийти без OK.
    if (b == MenuButton::Up)   editing_value += 1.0;
    if (b == MenuButton::Down) editing_value -= 1.0;

    if (b == MenuButton::Ok) {
        if (field == Field::Low) {
            channel.set_low_limit(editing_value);   // підтвердили нижню -> записуємо, йдемо далі
            field = Field::High;
            editing_value = channel.get_high_limit();
        } else if (editing_value > channel.get_low_limit()) {
            // FR-3: нижня межа < верхня межа; некоректне — відхиляємо (лишаємось на полі)
            channel.set_high_limit(editing_value);
            nav.pop();   // обидві межі збережено -> назад до списку каналів
        }
    }

    if (b == MenuButton::Back) nav.pop();   // вихід без збереження поточного (ще не підтвердженого) поля
}
