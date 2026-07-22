// ============================================================
//  MenuController.h
//  Стек сторінок меню. Без new/delete: стек — масив ВКАЗІВНИКІВ на вже
//  існуючі (глобальні/статичні) сторінки — уникає динамічного виділення
//  пам'яті на AVR із 2КБ RAM.
//
//  Сам тікає і резолвить 3 фізичні кнопки (Up/Down/Ok) у MenuButton:
//  довге утримання OK поза меню -> вхід (push entry_page); одиночний/
//  подвійний клік OK усередині меню -> Ok/Back. main.cpp лише оголошує
//  кнопки (піни — його турбота, як і з ChannelTemp) і кличе update().
// ============================================================
#pragma once

#include <EncButton.h>
#include "MenuPage.h"
#include "../Timer.h"

class MenuController {
private:
    static const int MAX_DEPTH = 4;   // з запасом; MVP потребує лише 2
    MenuPage* stack[MAX_DEPTH];
    int depth;                        // 0 = стек порожній = поза режимом меню
    LiquidCrystal_I2C& lcd;

    Button& btn_up;
    Button& btn_down;
    Button& btn_ok;
    MenuPage* entry_page;   // куди push() при довгому утриманні OK поза меню

    Timer idle_timer;   // бездіяльність у меню -> автовихід (розділ 4-А специфікації)

    MenuButton resolveButton();   // з поточних тіків кнопок -> яка menu-дія (лише всередині меню)

public:
    MenuController(LiquidCrystal_I2C& lcd_, Button& btn_up_, Button& btn_down_,
                   Button& btn_ok_, MenuPage* entry_page_);

    void push(MenuPage* page);   // зайти глибше (напр. зі списку каналів -> редагування меж)
    void pop();                  // повернутись на попередню (або спорожнити стек — вихід з меню)
    bool isEmpty() const;        // true -> режим меню закінчився

    void update();   // тікає кнопки; сам вирішує: увійти в меню / диспатчнути дію / нічого
};
