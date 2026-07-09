// ============================================================
//  ChannelListPage.h
//  Список вибору каналу. Керований даними: масив {підпис, дочірня
//  сторінка}, а не захардкоджені if channel == K1/K2/K3. Додати 4-й
//  канал — це додати запис у масив, не переписати цю сторінку.
// ============================================================
#pragma once

#include "MenuPage.h"

class ChannelListPage : public MenuPage {
public:
    struct Entry {
        const char* label;      // напр. "K1"
        MenuPage* edit_page;    // куди перейти по OK для цього пункту
    };

private:
    Entry* entries;       // масив пунктів (владіє ним викликач, не ця сторінка)
    int entry_count;
    int cursor;           // який пункт зараз підсвічений

public:
    ChannelListPage(Entry* entries_, int count);

    void onEnter() override;
    void render(LiquidCrystal_I2C& lcd) override;
    void onButton(MenuButton b, MenuController& nav) override;
};
