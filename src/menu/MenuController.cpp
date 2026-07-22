#include "MenuController.h"

namespace {
    const unsigned long IDLE_TIMEOUT_MS = 20000;   // розділ 4-А специфікації: автовихід за бездіяльності
}

MenuController::MenuController(LiquidCrystal_I2C& lcd_, Button& btn_up_, Button& btn_down_,
                                 Button& btn_ok_, MenuPage* entry_page_)
    : depth(0),
      lcd(lcd_),
      btn_up(btn_up_),
      btn_down(btn_down_),
      btn_ok(btn_ok_),
      entry_page(entry_page_),
      idle_timer(IDLE_TIMEOUT_MS)
{
    btn_ok.setHoldTimeout(1000);   // довге утримання OK (поза меню) -> вхід у меню
}

void MenuController::push(MenuPage* page) {
    if (depth >= MAX_DEPTH) return;   // запобіжник; на MVP-глибині ніколи не спрацює
    idle_timer.reset();   // вхід у сторінку — це теж активність
    stack[depth++] = page;
    lcd.clear();
    page->onEnter();
    page->render(lcd);
}

void MenuController::pop() {
    if (depth == 0) return;
    depth--;
    if (depth > 0) {
        // повернулись на сторінку, що вже існувала в стеку — перемалювати ЇЇ
        lcd.clear();
        stack[depth - 1]->render(lcd);
    }
    // якщо depth стало 0 — стек порожній; isEmpty() підкаже main.cpp вийти з меню
}

bool MenuController::isEmpty() const {
    return depth == 0;
}

// Ok/Back лежать на одній кнопці — розрізняються кількістю кліків. hasClicks(1/2)
// чекає таймаут кліку (~500мс), щоб відрізнити одиночний клік від початку
// подвійного — трохи повільніше за окрему кнопку "Назад", зате без 4-ї кнопки.
//
// Up/Down реагують і на press() (клік), і на step() — EncButton сам генерує
// step() періодично, поки кнопка затиснена довше hold-таймауту, тож утримання
// саме повторює Up/Down без окремого таймера тут. Сторінки, яким треба
// пришвидшення (LimitValueEditPage), рахують послідовні повтори самі.
MenuButton MenuController::resolveButton() {
    if (btn_ok.hasClicks(2))   return MenuButton::Back;
    if (btn_ok.hasClicks(1))   return MenuButton::Ok;
    if (btn_up.press()   || btn_up.step())   return MenuButton::Up;
    if (btn_down.press() || btn_down.step()) return MenuButton::Down;
    return MenuButton::None;
}

void MenuController::update() {
    btn_up.tick();
    btn_down.tick();
    btn_ok.tick();

    if (depth == 0) {
        if (btn_ok.hold()) push(entry_page);   // довге утримання OK поза меню -> вхід
        return;
    }

    MenuButton b = resolveButton();
    if (b == MenuButton::None) {
        // бездіяльність у меню -> автовихід (незбережене відхиляється саме тим,
        // що жодна сторінка не встигла отримати свій OK/Save)
        if (idle_timer.ready()) depth = 0;
        return;
    }
    idle_timer.reset();

    stack[depth - 1]->onButton(b, *this);   // сторінка сама мутує стан і/або push/pop
    if (depth > 0) stack[depth - 1]->render(lcd);   // рендер централізовано тут
}
