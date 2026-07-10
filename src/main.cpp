// ============================================================
//  src/main.cpp — режим моніторингу (3 канали температури, LCD 20x4 I2C)
// ============================================================
#include <Arduino.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <EncButton.h>
#include "ChannelTemp.h"
#include "MonitorRow.h"
#include "Timer.h"
#include "menu/MenuButton.h"
#include "menu/MenuController.h"
#include "menu/ChannelListPage.h"
#include "menu/limit/LimitSelectPage.h"
#include "menu/limit/LimitValueEditPage.h"

// !!! Адреса LCD зі сканера (0x27 або 0x3F) !!!
#define LCD_ADDR 0x27

LiquidCrystal_I2C lcd(LCD_ADDR, 20, 4);

// Піни термісторів (аналогові входи Nano). Межі — тимчасові значення
// за замовчуванням (FR-3/NFR-1), поки немає введення з кнопок.
ChannelTemp ch1(A0, 10.0, 30.0);
ChannelTemp ch2(A1, 10.0, 30.0);
ChannelTemp ch3(A2, 10.0, 30.0);

// Рядки 0..2 — по одному каналу; рядок 3 — під заголовок (FR-2).
MonitorRow row1(lcd, 0, "K1");
MonitorRow row2(lcd, 1, "K2");
MonitorRow row3(lcd, 2, "K3");

// FR-1: опитування каналів не рідше 2 рази/сек -> інтервал 500 мс.
Timer measure_timer(500);

// ---- Режим налаштування (розділ 4-А) ----------------------------------
// !!! ТИМЧАСОВІ піни кнопок — постав свої під реальне підключення !!!
// Лише 3 кнопки: "Назад" — це подвійний клік OK, окремої кнопки не треба.
#define BTN_UP_PIN   2
#define BTN_DOWN_PIN 3
#define BTN_OK_PIN   4

// Button(pin) за замовчуванням: INPUT_PULLUP, активний рівень LOW —
// точно відповідає підключенню "кнопка на GND" без зовнішніх резисторів.
Button btnUp(BTN_UP_PIN);
Button btnDown(BTN_DOWN_PIN);
Button btnOk(BTN_OK_PIN);

LimitValueEditPage editValuePage1(ch1);
LimitSelectPage    selectPage1(ch1, editValuePage1);
LimitValueEditPage editValuePage2(ch2);
LimitSelectPage    selectPage2(ch2, editValuePage2);
LimitValueEditPage editValuePage3(ch3);
LimitSelectPage    selectPage3(ch3, editValuePage3);
ChannelListPage::Entry channelEntries[] = {
    { "K1", &selectPage1 },
    { "K2", &selectPage2 },
    { "K3", &selectPage3 },
};
ChannelListPage channelListPage(channelEntries, 3);
MenuController menu(lcd, btnUp, btnDown, btnOk, &channelListPage);

void setup() {
    Serial.begin(115200);
    lcd.init();
    lcd.backlight();
    MonitorRow::registerChars(lcd);   // CGRAM-стрілки ^/v для drawStatus()

    lcd.setCursor(0, 3);
    lcd.print("== TEMP MONITOR ==");
}

void loop() {
    if (measure_timer.ready()) {
        ch1.update();
        ch2.update();
        ch3.update();
    }

    bool was_in_menu = !menu.isEmpty();
    menu.update();   // сам тікає кнопки; вирішує вхід у меню / навігацію всередині

    if (menu.isEmpty()) {
        if (was_in_menu) {
            // щойно вийшли з меню — MonitorRow-кеш застарілий відносно щойно
            // очищеного екрана, тому перемальовуємо статику наново
            lcd.clear();
            row1.drawFrame(ch1.get_low_limit(), ch1.get_high_limit());
            row2.drawFrame(ch2.get_low_limit(), ch2.get_high_limit());
            row3.drawFrame(ch3.get_low_limit(), ch3.get_high_limit());
        } else {
            // render() викликається щоцикл — так власний таймер блимання
            // кожного рядка (MonitorRow) реагує вчасно, незалежно від
            // того, як часто реально оновлюються виміри.
            row1.render(ch1.get_temp(), ch1.status(), ch1.get_low_limit(), ch1.get_high_limit());
            row2.render(ch2.get_temp(), ch2.status(), ch2.get_low_limit(), ch2.get_high_limit());
            row3.render(ch3.get_temp(), ch3.status(), ch3.get_low_limit(), ch3.get_high_limit());
        }
    }
}
