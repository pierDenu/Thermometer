// ============================================================
//  MonitorRow.h
//  Клас, ОБ'ЄКТ якого відповідає за один рядок тексту на LCD
//  у режимі моніторингу (один канал = один рядок = один об'єкт).
//
//  Кожен об'єкт сам:
//   - знає свій рядок і підпис ("K1")
//   - пам'ятає, що зараз намальовано (перемальовує лише за зміни)
//   - тримає фіксовану ширину полів (без "привидів")
//   - блимає ЦІЛИМ рядком у стані тривоги; фазу перемикає toggle(),
//     який викликається ЗА ВЛАСНИМ таймером усередині render() —
//     рядки блимають незалежно один від одного (легка розсинхронізація
//     між рядками — не критично)
// ============================================================
#ifndef MONITORROW_H
#define MONITORROW_H

#include <Arduino.h>
#include <LiquidCrystal_I2C.h>
#include "Timer.h"
#include "Status.h"

class MonitorRow {
private:
    LiquidCrystal_I2C& lcd;   // ПОСИЛАННЯ на спільний дисплей (не копія!)
    int row;                  // номер рядка на LCD (0..3)
    const char* label;        // підпис каналу, напр. "K1"

    // пам'ять "що зараз намальовано" — для перемальовування лише за зміни
    float  shown_temp;
    Status shown_status;

    bool  is_shown;           // фаза блимання цілого рядка (true = видимо)
    Timer blink_timer;        // власний таймер цієї фази

    // позиції полів у рядку (спільні для всіх рядків)
    static const int TEMP_COL   = 3;   // значення — одразу після підпису
    static const int LIMITS_COL = 10;   // межі — там, де раніше було значення
    static const int UNIT_COL   = 8;
    static const int STATUS_COL = 16;

    // "неможливі" значення-мітки для shown_temp (сигналізують стан поля)
    static constexpr float NONE   = -1000000.0;  // ще нічого не малювали
    static constexpr float HIDDEN = -1000001.0;  // поле сховане (фаза блимання)
    static constexpr float DASHES = -1000002.0;  // показано "--.-" (ERR)

    void drawValue(float temp, Status status);  // число або "--.-", лише за зміни
    void hideValue();                           // затерти поле (фаза блимання)
    void drawStatus(Status s);                  // OK / стрілка / ERR, лише за зміни
    void printLimit(float v);                   // одна межа, вирівняна під 2 знаки

    // межі (low/high) передаються ЗЗОВНІ щоразу, не зберігаються в об'єкті
    void toggle(float temp, Status status, float low_limit, float high_limit);
    void update(float temp, Status status);  // об'єднує drawStatus + drawValue/hideValue

public:
    // коди CGRAM-символів стрілок (спільні для registerChars() і drawStatus(),
    // щоб не розсинхронізувались)
    static const uint8_t CHAR_ARROW_UP   = 0;
    static const uint8_t CHAR_ARROW_DOWN = 1;

    MonitorRow(LiquidCrystal_I2C& lcd_, int row_, const char* label_,
               unsigned long blink_interval_ms = 500);

    // зареєструвати CGRAM-символи стрілок — викликати РАЗ у setup(), до
    // першого використання drawStatus()/render()
    static void registerChars(LiquidCrystal_I2C& lcd);

    void drawFrame(float low_limit, float high_limit);  // статика рядка — РАЗ при вході в режим
    void render(float temp, Status status, float low_limit, float high_limit);  // виклик у циклі
};

#endif
