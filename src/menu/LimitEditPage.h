// ============================================================
//  LimitEditPage.h
//  Редагування нижньої, потім верхньої межі ОДНОГО каналу (FR-3:
//  обрати канал -> нижня межа -> верхня межа -> зберегти).
// ============================================================
#pragma once

#include "MenuPage.h"
#include "../ChannelTemp.h"

class LimitEditPage : public MenuPage {
private:
    ChannelTemp& channel;                   // чиї межі редагуємо
    enum class Field { Low, High } field;   // на якому полі зараз стоїмо
    float editing_value;                    // значення, що редагується (ще НЕ записане в channel)

public:
    explicit LimitEditPage(ChannelTemp& channel_);

    void onEnter() override;
    void render(LiquidCrystal_I2C& lcd) override;
    void onButton(MenuButton b, MenuController& nav) override;
};
