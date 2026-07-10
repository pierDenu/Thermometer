#include "LimitSelectPage.h"
#include "LimitValueEditPage.h"
#include "MenuController.h"

LimitSelectPage::LimitSelectPage(ChannelTemp& channel_, LimitValueEditPage& edit_page_)
    : LimitPageBase(channel_), edit_page(edit_page_) {}

void LimitSelectPage::onEnter() {
    field = Field::Low;   // щоразу при вході курсор на нижній межі
}

void LimitSelectPage::drawAll(LiquidCrystal_I2C& lcd) {
    drawRow(lcd, 0, "Nyzhnya:  ", channel.get_low_limit(),  field == Field::Low);
    drawRow(lcd, 1, "Verkhnya: ", channel.get_high_limit(), field == Field::High);
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
