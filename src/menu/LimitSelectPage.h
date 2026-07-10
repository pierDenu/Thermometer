// ============================================================
//  LimitSelectPage.h
//  Вибір, яку межу каналу редагувати (FR-3): обидві межі завжди
//  видимі, Up/Down переміщують курсор між ними, OK передає обране
//  поле в LimitValueEditPage і заходить туди (push), Back виходить
//  зі сторінки цілком (pop, повертає на ChannelListPage).
// ============================================================
#pragma once

#include "LimitPageBase.h"

class LimitValueEditPage;

class LimitSelectPage : public LimitPageBase {
private:
    LimitValueEditPage& edit_page;   // куди push() по OK

protected:
    void drawAll(LiquidCrystal_I2C& lcd) override;

public:
    LimitSelectPage(ChannelTemp& channel_, LimitValueEditPage& edit_page_);

    void onEnter() override;
    void onButton(MenuButton b, MenuController& nav) override;
};
