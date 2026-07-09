// ============================================================
//  MonitorRow.cpp — реалізація класу MonitorRow
// ============================================================
#include "MonitorRow.h"

// Конструктор. Зверни увагу: посилання lcd МОЖНА ініціалізувати
// ЛИШЕ через список ініціалізації (посилання не можна "присвоїти" в тілі) —
// це та сама причина, що й з полем-об'єктом у ChannelTemp.
MonitorRow::MonitorRow(LiquidCrystal_I2C& lcd_, int row_, const char* label_,
                       unsigned long blink_interval_ms)
    : lcd(lcd_),
      row(row_),
      label(label_),
      shown_temp(NONE),
      shown_status(Status::Ok),
      is_shown(false),
      blink_timer(blink_interval_ms) {}

// Одна межа, вирівняна під 2 знаки (напр. "10", " 5"). Межі поки завжди
// 2-значні (дефолти на кшталт 10/30) — ширший діапазон ("-20"/"105")
// зламає вирівнювання; коли з'явиться реальне введення меж, доведеться
// переглянути.
void MonitorRow::printLimit(float v) {
    if (v < 10) lcd.print(" ");
    lcd.print(v, 0);
}

// Статика рядка: підпис + межі (тепер там, де раніше було значення) + одиниця.
// Викликається один раз при вході в режим (після lcd.clear()) і повторно
// з toggle(), коли рядок знову стає видимим після стирання.
void MonitorRow::drawFrame(float low_limit, float high_limit) {
    lcd.setCursor(0, row);
    lcd.print(label);

    lcd.setCursor(LIMITS_COL, row);
    printLimit(low_limit);
    lcd.print("-");
    printLimit(high_limit);

    lcd.setCursor(UNIT_COL, row);
    lcd.print("C");

    shown_temp   = NONE;         // забути намальоване -> update перемалює
    shown_status = Status::Ok;   // (умовне; реальний статус прийде в update)
}

// Малює значення: число (фіксована ширина 5) або "--.-" при ERR.
// Порівнює з тим, що вже показано, і виходить, якщо не змінилось.
void MonitorRow::drawValue(float temp, Status status) {
    // яке "значення" зараз має бути на екрані
    float target = (status == Status::Error) ? DASHES : temp;
    if (target == shown_temp) return;   // не змінилось — нічого не шлемо

    lcd.setCursor(TEMP_COL, row);
    if (status == Status::Error) {
        lcd.print(" --.-");             // 5 знаків, як і число
    } else {
        // округлюємо ДО перевірки ширини — інакше, напр. 99.96 пройде
        // перевірку як "< 100" (додасть пробіл), а надрукується як "100.0"
        // (округлення в lcd.print теж на 1 знак) -> 6 символів замість 5
        float rounded = roundf(temp * 10.0f) / 10.0f;
        if (rounded < 100.0 && rounded > -10.0) lcd.print(" ");  // вирівнювання до 5
        lcd.print(rounded, 1);
    }
    shown_temp = target;
}

// Ховає поле значення (фаза блимання "вимкнено"). Теж лише за зміни:
// якщо вже сховано — повторно пробіли не шлемо.
void MonitorRow::hideValue() {
    if (shown_temp == HIDDEN) return;
    lcd.setCursor(TEMP_COL, row);
    lcd.print("     ");                 // 5 пробілів
    shown_temp = HIDDEN;
}

// Індикатор стану праворуч. Фіксована ширина 3 знаки -> без "привидів".
// Стрілки — CGRAM-символи (registerChars(), коди CHAR_ARROW_UP/DOWN).
void MonitorRow::drawStatus(Status s) {
    if (s == shown_status) return;

    lcd.setCursor(STATUS_COL, row);
    switch (s) {
        case Status::Ok:      lcd.print("OK "); break;
        case Status::TooHigh: lcd.write(CHAR_ARROW_UP);   lcd.print("  "); break;
        case Status::TooLow:  lcd.write(CHAR_ARROW_DOWN); lcd.print("  "); break;
        case Status::Error:   lcd.print("ERR"); break;
    }
    shown_status = s;
}

// Реєструє два власні символи (CGRAM) — стрілки вгору/вниз. HD44780 не має
// їх у стандартному ПЗУ. Викликати РАЗ у setup(), до першого render().
void MonitorRow::registerChars(LiquidCrystal_I2C& lcd) {
    uint8_t arrow_up[8] = {
        0b00100,
        0b01110,
        0b11111,
        0b00100,
        0b00100,
        0b00100,
        0b00100,
        0b00000
    };
    uint8_t arrow_down[8] = {
        0b00000,
        0b00100,
        0b00100,
        0b00100,
        0b00100,
        0b11111,
        0b01110,
        0b00100
    };
    lcd.createChar(CHAR_ARROW_UP, arrow_up);
    lcd.createChar(CHAR_ARROW_DOWN, arrow_down);
}

// Об'єднує drawStatus + drawValue/hideValue. Фаза блимання (is_shown)
// береться з внутрішнього стану — перемикає її toggle().
void MonitorRow::update(float temp, Status status) {
    drawStatus(status);   // статус показуємо завжди (не блимає — стрілка має бути видна)

    bool alarming = (status != Status::Ok);

    if (alarming && !is_shown) {
        hideValue();              // фаза "вимкнено" -> ховаємо число
    } else {
        drawValue(temp, status);  // показуємо число (або "--.-")
    }
}

// Перемикає фазу блимання і стирає/малює ЦІЛИЙ рядок (20 символів).
// temp/status/межі приходять параметрами від render() (той самий виклик,
// що й спричинив toggle) — нічого зайвого не зберігаємо в об'єкті.
void MonitorRow::toggle(float temp, Status status, float low_limit, float high_limit) {
    is_shown = !is_shown;

    if (is_shown) {
        drawFrame(low_limit, high_limit);
        update(temp, status);
    } else {
        // фаза "сховати" — стираємо ввесь рядок одразу
        lcd.setCursor(0, row);
        lcd.print("                    ");  // 20 пробілів
        shown_temp = HIDDEN;
    }
}

// Єдиний публічний виклик у циклі. Якщо рядок у тривозі й спрацював його
// власний таймер блимання — перемикає фазу через toggle() (цілий рядок).
// Інакше — звичайне update() (лише те, що змінилось).
void MonitorRow::render(float temp, Status status, float low_limit, float high_limit) {
    bool alarming = (status != Status::Ok);

    if (alarming) {
        if (blink_timer.ready()) {
            toggle(temp, status, low_limit, high_limit);
        }
    } else {
        if (!is_shown) {
            // тривога минула саме тоді, коли рядок був стертий -> toggle()
            // більше не викличеться (alarming вже false) — відновлюємо самі
            drawFrame(low_limit, high_limit);
            is_shown = true;
        }
        update(temp, status);
    }
}
