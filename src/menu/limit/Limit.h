// ============================================================
//  Limit.h
//  Редагування меж каналу (FR-3), три класи разом — вони існують
//  лише один заради одного і завжди компілюються/розуміються як
//  одне ціле, тож окремі файли на клас тут давали б лише накладні
//  витрати (і форвард-декларацію, якої так не треба):
//
//   - LimitPageBase   — спільний рендер (drawRow(): курсор "> ...<")
//   - LimitValueEditPage — редагування ОДНОГО обраного значення
//   - LimitSelectPage    — вибір, яку межу редагувати; push() у
//                          LimitValueEditPage і назад через pop()
// ============================================================
#pragma once

#include "../MenuPage.h"
#include "../../ChannelTemp.h"

// ---- LimitPageBase --------------------------------------------------
class LimitPageBase : public MenuPage {
public:
    enum class Field { Low, High };

protected:
    ChannelTemp& channel;   // чиї межі показуємо/редагуємо
    Field field;            // яке поле зараз обране (курсор або те, що редагується)

    explicit LimitPageBase(ChannelTemp& channel_);

    // Значення фіксованою шириною 5 символів (вкладається "-20.0".."105.0"
    // з FR-1) — без цього коротше число лишало б "привида" від
    // попереднього довшого при перемальовуванні.
    void printField(LiquidCrystal_I2C& lcd, float v);

    // Один рядок межі: курсор "> " зліва від обраного поля завжди; "<"
    // справа — лише коли showEditMarker() каже, що це саме редагування
    // (LimitSelectPage — просто вибір, без "<"; LimitValueEditPage — так,
    // щоб позначити "це значення зараз змінюється").
    void drawRow(LiquidCrystal_I2C& lcd, int row, const char* label, float v, bool selected);

    virtual bool showEditMarker() const { return false; }

    // Конкретний вигляд сторінки (які значення й яке поле обране) —
    // визначає кожен нащадок; render() лише викликає це.
    virtual void drawAll(LiquidCrystal_I2C& lcd) = 0;

public:
    void render(LiquidCrystal_I2C& lcd) override final;
};

// ---- LimitValueEditPage ----------------------------------------------
// Редагування ОДНОГО обраного значення межі: вигляд той самий, що й у
// LimitSelectPage (той самий drawRow()), але Up/Down тепер міняють
// значення обраного поля, а не переміщують курсор. OK валідує (нижня
// межа < верхня) і записує в channel, Back виходить без збереження —
// обидва повертаються на LimitSelectPage через nav.pop() (він
// лишається нижче в стеку).
class LimitValueEditPage : public LimitPageBase {
private:
    float editing_value;   // значення під час редагування (ще НЕ записане в channel)

protected:
    void drawAll(LiquidCrystal_I2C& lcd) override;
    bool showEditMarker() const override { return true; }   // "<" справа -> це значення редагується

public:
    explicit LimitValueEditPage(ChannelTemp& channel_);

    // Викликає LimitSelectPage ПЕРЕД nav.push(this) — готує, яке поле
    // і з яким стартовим значенням редагувати.
    void beginEditing(Field f);

    void onEnter() override {}   // стан уже підготовлено в beginEditing()
    void onButton(MenuButton b, MenuController& nav) override;
};

// ---- LimitSelectPage --------------------------------------------------
// Вибір, яку межу каналу редагувати: обидві межі завжди видимі, Up/Down
// переміщують курсор між ними, OK передає обране поле в
// LimitValueEditPage і заходить туди (push), Back виходить зі сторінки
// цілком (pop, повертає на ChannelListPage).
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
