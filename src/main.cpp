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
#include "menu/limit/Limit.h"

// !!! Адреса LCD зі сканера (0x27 або 0x3F) !!!
#define LCD_ADDR 0x27

LiquidCrystal_I2C lcd(LCD_ADDR, 20, 4);

const int NUM_CHANNELS = 3;

// Все про один канал разом: датчик+межі (ChannelTemp), обидві сторінки
// редагування меж і рядок LCD у режимі моніторингу. Без цього довелось би
// тримати editValuePageN/selectPageN/rowN окремими глобалами на кожен
// канал — N копій однакового коду замість N рядків даних (як уже зроблено
// для channelEntries).
struct ChannelSlot {
    ChannelTemp channel;
    LimitValueEditPage edit_page;
    LimitSelectPage select_page;
    MonitorRow row;
    const char* label;

    // edit_page/select_page посилаються на "сусідні" поля цього ж об'єкта —
    // порядок ІНІЦІАЛІЗАЦІЇ визначає порядок ОГОЛОШЕННЯ полів вище (не
    // порядок нижче), тому channel має лишатись першим полем. row від
    // сусідніх полів не залежить (бере lcd/label_/lcd_row напряму з
    // параметрів конструктора), тому для нього це не критично.
    ChannelSlot(int pin, float low, float high, const char* label_,
                LiquidCrystal_I2C& lcd, int lcd_row)
        : channel(pin, low, high),
          edit_page(channel),
          select_page(channel, edit_page),
          row(lcd, lcd_row, label_),
          label(label_)
    {}
};

// Піни термісторів (аналогові входи Nano). Межі — тимчасові значення
// за замовчуванням (FR-3/NFR-1), поки немає введення з кнопок.
// Рядки LCD 0..2 — по одному каналу; рядок 3 — під заголовок (FR-2).
ChannelSlot slots[NUM_CHANNELS] = {
    ChannelSlot(A0, 10.0, 30.0, "K1", lcd, 0),
    ChannelSlot(A1, 10.0, 30.0, "K2", lcd, 1),
    ChannelSlot(A2, 10.0, 30.0, "K3", lcd, 2),
};

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
    MonitorRow::registerChars(lcd);   // CGRAM-стрілки ^/v для drawStatus()

    lcd.setCursor(0, 3);
    lcd.print("== TEMP MONITOR ==");
}

void loop() {
    if (measure_timer.ready()) {
        for (int i = 0; i < NUM_CHANNELS; i++) slots[i].channel.update();
    }

    bool was_in_menu = !menu.isEmpty();
    menu.update();   // сам тікає кнопки; вирішує вхід у меню / навігацію всередині

    if (menu.isEmpty()) {
        if (was_in_menu) {
            // щойно вийшли з меню — MonitorRow-кеш застарілий відносно щойно
            // очищеного екрана, тому перемальовуємо статику наново
            lcd.clear();
            for (int i = 0; i < NUM_CHANNELS; i++) {
                slots[i].row.drawFrame(slots[i].channel.get_low_limit(), slots[i].channel.get_high_limit());
            }
        } else {
            // render() викликається щоцикл — так власний таймер блимання
            // кожного рядка (MonitorRow) реагує вчасно, незалежно від
            // того, як часто реально оновлюються виміри.
            for (int i = 0; i < NUM_CHANNELS; i++) {
                ChannelTemp& ch = slots[i].channel;
                slots[i].row.render(ch.get_temp(), ch.status(), ch.get_low_limit(), ch.get_high_limit());
            }
        }
    }
}
