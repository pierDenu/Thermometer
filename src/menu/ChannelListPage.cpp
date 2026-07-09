#include "ChannelListPage.h"
#include "MenuController.h"

ChannelListPage::ChannelListPage(Entry* entries_, int count)
    : entries(entries_), entry_count(count), cursor(0) {}

void ChannelListPage::onEnter() {
    cursor = 0;   // завжди починаємо список зверху
}

void ChannelListPage::render(LiquidCrystal_I2C& lcd) {
    lcd.setCursor(0, 0);
    lcd.print("Obery kanal:");
    for (int i = 0; i < entry_count; i++) {
        lcd.setCursor(0, i + 1);
        lcd.print(i == cursor ? "> " : "  ");
        lcd.print(entries[i].label);
    }
}

void ChannelListPage::onButton(MenuButton b, MenuController& nav) {
    if (b == MenuButton::Down) cursor = (cursor + 1) % entry_count;
    if (b == MenuButton::Up)   cursor = (cursor - 1 + entry_count) % entry_count;

    if (b == MenuButton::Ok)   nav.push(entries[cursor].edit_page);
    if (b == MenuButton::Back) nav.pop();   // найвищий рівень меню -> спорожнить стек -> вихід
}
