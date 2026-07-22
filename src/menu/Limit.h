// ============================================================
//  Limit.h — редагування меж каналу (FR-3): LimitPageBase (спільний
//  рендер), LimitValueEditPage (правка одного поля), LimitSelectPage
//  (вибір поля + Save).
// ============================================================
#pragma once

#include "MenuPage.h"
#include "../ChannelTemp.h"

// ---- LimitPageBase --------------------------------------------------
class LimitPageBase : public MenuPage {
public:
    enum class Field { Low, High, Save };   // Save — лише для LimitSelectPage

protected:
    ChannelTemp& channel;
    Field field;   // поле, що редагується
    int cursor;    // рядок LCD (0/1/2), де малюються "> " і " <"

    explicit LimitPageBase(ChannelTemp& channel_);

    void drawValue(LiquidCrystal_I2C& lcd, Field f);   // фіксовані 5 символів, без "привида" від коротшого числа
    void drawRow(LiquidCrystal_I2C& lcd, int row, const char* label, Field f);

    virtual bool showEditMarker() const { return false; }
    virtual float valueFor(Field f) const;

    void drawSaveRow(LiquidCrystal_I2C& lcd);
    void drawCursor(LiquidCrystal_I2C& lcd);
    void drawEditMarker(LiquidCrystal_I2C& lcd);

    bool isFieldDirty(Field f) const;
    bool save_available() const { return isFieldDirty(Field::Low) || isFieldDirty(Field::High); }
public:
    void render(LiquidCrystal_I2C& lcd) override;   // сам чистить екран — без цього курсор лишав би "привида"
};

// ---- LimitValueEditPage ----------------------------------------------
class LimitValueEditPage : public LimitPageBase {
private:
    float editing_value;   // ще не записане в channel, поки нема OK

protected:
    bool showEditMarker() const override { return true; }
    float valueFor(Field f) const override;

public:
    explicit LimitValueEditPage(ChannelTemp& channel_);

    void beginEditing(Field f);   // кличе LimitSelectPage перед nav.push(this)

    void onEnter() override {}
    void onButton(MenuButton b, MenuController& nav) override;
};

// ---- LimitSelectPage --------------------------------------------------
class LimitSelectPage : public LimitPageBase {
private:
    LimitValueEditPage& edit_page;
    bool save_requested;   // true після OK на "Save" — main.cpp сам кличе EepromLimitStore і скидає прапорець
    static Field fieldForCursor(int c);

public:
    LimitSelectPage(ChannelTemp& channel_, LimitValueEditPage& edit_page_);

    void onEnter() override;
    void onButton(MenuButton b, MenuController& nav) override;

    bool saveRequested() const { return save_requested; }
    void clearSaveRequest() { save_requested = false; }
};
