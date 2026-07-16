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
    // Save — "віртуальне" поле лише для LimitSelectPage (курсор на пункті
    // Save, рядок 2). LimitValueEditPage його ніколи не отримує —
    // beginEditing() і onButton() там завжди мають справу лише з Low/High.
    enum class Field { Low, High, Save };

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

    // Чи показувати курсор на полі f — за замовчуванням "це і є обране
    // поле". Коли field == Save (LimitSelectPage), це саме по собі дає
    // false і для Low, і для High — окремого перевизначення не треба.
    virtual bool isFieldSelected(Field f) const { return field == f; }

    // Чи курсор зараз на пункті "Save" (рядок 2). За замовчуванням false —
    // рядок і надалі малюється (для однакового вигляду обох сторінок),
    // просто без курсора; лише LimitSelectPage може зробити його true.
    virtual bool isSaveSelected() const { return false; }

    // Яке значення показувати для поля f — за замовчуванням поточне з
    // channel; LimitValueEditPage підміняє його на editing_value для
    // поля, що зараз редагується.
    virtual float valueFor(Field f) const;

    // Рядки 0/1 (Low/High) — спільні для обох сторінок.
    void drawLimitRows(LiquidCrystal_I2C& lcd);

    // Рядок 2 ("Save") — теж спільний, щоб обидві сторінки мали однаковий
    // каркас і перехід між ними виглядав безшовно.
    void drawSaveRow(LiquidCrystal_I2C& lcd);

public:
    void render(LiquidCrystal_I2C& lcd) override;
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
    bool showEditMarker() const override { return true; }   // "<" справа -> це значення редагується
    float valueFor(Field f) const override;                 // поле, що редагується -> editing_value

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
    int cursor;                      // 0..2 -> позиція курсора (Low/High/Save)
    bool save_requested;             // true одразу після OK на "Save" — до clearSaveRequest()

    // Мапить позицію курсора на Field. Окрема функція, а не ланцюжок
    // if/else if, — Up/Down рухають cursor по колу через % (як у
    // ChannelListPage), а field лише синхронізується під нього.
    static Field fieldForCursor(int c);

protected:
    bool isSaveSelected() const override { return cursor == 2; }

public:
    LimitSelectPage(ChannelTemp& channel_, LimitValueEditPage& edit_page_);

    void onEnter() override;
    void onButton(MenuButton b, MenuController& nav) override;

    // Ця сторінка нічого не знає про EEPROM — вона лише сигналізує момент
    // "Save" підняттям прапорця; main.cpp щоцикл перевіряє його й сам
    // кличе EepromLimitStore::save(), тоді скидає прапорець назад.
    bool saveRequested() const { return save_requested; }
    void clearSaveRequest() { save_requested = false; }
};
