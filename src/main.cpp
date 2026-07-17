// ============================================================
//  src/main.cpp — режим моніторингу (3 канали температури, LCD 20x4 I2C)
// ============================================================
#include <Arduino.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <EncButton.h>
#include "ChannelTemp.h"
#include "EepromLimitStore.h"
#include "MonitorRow.h"
#include "Timer.h"
#include "menu/MenuButton.h"
#include "menu/MenuController.h"
#include "menu/ChannelListPage.h"
#include "menu/limit/Limit.h"

// !!! Адреса LCD зі сканера (0x27 або 0x3F) !!!
#define LCD_ADDR 0x27

LiquidCrystal_I2C lcd(LCD_ADDR, 20, 4);

const int NUM_CHANNELS = 3;

struct ChannelSlot {
    ChannelTemp channel;
    LimitValueEditPage edit_page;
    LimitSelectPage select_page;
    MonitorRow row;
    const char* label;

    // Порядок ІНІЦІАЛІЗАЦІЇ = порядок ОГОЛОШЕННЯ полів вище (не порядок
    // нижче) — channel має лишатись першим полем.
    ChannelSlot(int pin, float low, float high, const char* label_,
                LiquidCrystal_I2C& lcd, int lcd_row)
        : channel(pin, low, high),
          edit_page(channel),
          select_page(channel, edit_page),
          row(lcd, lcd_row, label_),
          label(label_)
    {}
};

ChannelSlot slots[NUM_CHANNELS] = {
    ChannelSlot(A0, 10.0, 30.0, "K1", lcd, 0),
    ChannelSlot(A1, 10.0, 30.0, "K2", lcd, 1),
    ChannelSlot(A2, 10.0, 30.0, "K3", lcd, 2),
};

Timer measure_timer(500);

const int EEPROM_ADDR_STRIDE = 16;   // з запасом більше за sizeof(Record) в EepromLimitStore.cpp

// !!! ТИМЧАСОВІ піни кнопок — постав свої під реальне підключення !!!
#define BTN_UP_PIN   2
#define BTN_DOWN_PIN 3
#define BTN_OK_PIN   4

Button btnUp(BTN_UP_PIN);
Button btnDown(BTN_DOWN_PIN);
Button btnOk(BTN_OK_PIN);

ChannelListPage::Entry channelEntries[NUM_CHANNELS] = {
    { slots[0].label, &slots[0].select_page },
    { slots[1].label, &slots[1].select_page },
    { slots[2].label, &slots[2].select_page },
};
ChannelListPage channelListPage(channelEntries, NUM_CHANNELS);
MenuController menu(lcd, btnUp, btnDown, btnOk, &channelListPage);

void setup() {
    Serial.begin(115200);
    lcd.init();
    lcd.backlight();
    MonitorRow::registerChars(lcd);

    lcd.setCursor(0, 3);
    lcd.print("== TEMP MONITOR ==");
}

void loop() {
    if (measure_timer.ready()) {
        for (int i = 0; i < NUM_CHANNELS; i++) slots[i].channel.update();
    }

    bool was_in_menu = !menu.isEmpty();
    menu.update();

    for (int i = 0; i < NUM_CHANNELS; i++) {
        if (slots[i].select_page.saveRequested()) {
            EepromLimitStore::save(slots[i].channel, i * EEPROM_ADDR_STRIDE);
            slots[i].select_page.clearSaveRequest();
        }
    }

    if (menu.isEmpty()) {
        if (was_in_menu) {
            lcd.clear();   // MonitorRow-кеш застарілий відносно щойно очищеного екрана
            for (int i = 0; i < NUM_CHANNELS; i++) {
                slots[i].row.drawFrame(slots[i].channel.get_low_limit(), slots[i].channel.get_high_limit());
            }
        } else {
            for (int i = 0; i < NUM_CHANNELS; i++) {
                ChannelTemp& ch = slots[i].channel;
                slots[i].row.render(ch.get_temp(), ch.status(), ch.get_low_limit(), ch.get_high_limit());
            }
        }
    }
}
