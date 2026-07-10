// ============================================================
//  LimitValueEditPage.h
//  Редагування ОДНОГО обраного значення межі (FR-3): вигляд той
//  самий, що й у LimitSelectPage (той самий drawRow() з базового
//  класу), але Up/Down тепер міняють значення обраного поля, а не
//  переміщують курсор. OK валідує (нижня межа < верхня) і записує
//  в channel, Back виходить без збереження — обидва повертаються
//  на LimitSelectPage через nav.pop() (він лишається нижче в стеку).
// ============================================================
#pragma once

#include "LimitPageBase.h"

class LimitValueEditPage : public LimitPageBase {
private:
    float editing_value;   // значення під час редагування (ще НЕ записане в channel)

protected:
    void drawAll(LiquidCrystal_I2C& lcd) override;

public:
    explicit LimitValueEditPage(ChannelTemp& channel_);

    // Викликає LimitSelectPage ПЕРЕД nav.push(this) — готує, яке поле
    // і з яким стартовим значенням редагувати.
    void beginEditing(Field f);

    void onEnter() override {}   // стан уже підготовлено в beginEditing()
    void onButton(MenuButton b, MenuController& nav) override;
};
