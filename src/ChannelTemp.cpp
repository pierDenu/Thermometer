#include "ChannelTemp.h"

#define DIVIDER_RESISTOR_OHM 1300.0       // опір резистора дільника, Ом
#define NTC_NOMINAL_RESISTANCE_OHM 10000.0 // опір NTC при номінальній температурі (R0), Ом
#define NTC_B_COEFFICIENT 3950.0         // коефіцієнт B терморезистора
#define NTC_NOMINAL_TEMP_C 25.0          // номінальна температура (T0), °C
#define KELVIN_OFFSET 273.15             // зсув для переведення °C у Кельвіни
#define HYSTERESIS_C 0.5                 // запас перед зняттям тривоги (FR-4)
#define FAULT_TEMP_C -50.0               // нижче — обрив/коротке (FR-5), не реальна температура

ChannelTemp::ChannelTemp(int pin, float low, float high)
    : thermistor_pin(pin),  // поле-об'єкт будується ТУТ
      low_limit(low),
      high_limit(high),
      curr_temp(0),
      state(Status::Ok)
{
  // тіло вже може бути порожнім
}

void ChannelTemp::set_low_limit(float new_limit) {
    low_limit = new_limit;
}

void ChannelTemp::set_high_limit(float new_limit) {
    high_limit = new_limit;
}

float ChannelTemp::get_low_limit() {
    return low_limit;
}

float ChannelTemp::get_high_limit() {
    return high_limit;
}

void ChannelTemp::update() {
    curr_temp = measure_temp();
}

// Гістерезис (FR-4): тривога знімається лише коли температура повернеться
// в межі із запасом HYSTERESIS_C, щоб не "дрижати" на самій межі.
// FR-5: обрив/коротке заганяють дільник у крайнє положення, а формулу
// Стейнхарта-Гарта — у нефізичне значення (curr_temp << реального мінімуму) —
// це й трактуємо як несправність датчика.
Status ChannelTemp::status() {
    if (curr_temp < FAULT_TEMP_C) state = Status::Error;
    if (curr_temp >= FAULT_TEMP_C && state == Status::Error) state = Status::Ok;

    if (state == Status::TooHigh && curr_temp <= high_limit - HYSTERESIS_C) state = Status::Ok;
    if (state == Status::TooLow  && curr_temp >= low_limit  + HYSTERESIS_C) state = Status::Ok;
    if (state == Status::Ok && curr_temp > high_limit) state = Status::TooHigh;
    if (state == Status::Ok && curr_temp < low_limit)  state = Status::TooLow;
    return state;
}

float ChannelTemp::get_temp() {
    return curr_temp;
}

float ChannelTemp::measure_temp() {
    int raw = analogRead(thermistor_pin);                       // 0..1023
    float voltage = raw * 5.0 / 1023.0;              // у вольтах (Nano = 5В)
    float resistance = DIVIDER_RESISTOR_OHM * voltage / (5.0 - voltage);  // опір NTC із дільника
    // T = 1 / ( 1/T0 + (1/B)*ln(R/R0) ),  температури в Кельвінах
    float steinhart = log(resistance / NTC_NOMINAL_RESISTANCE_OHM) / NTC_B_COEFFICIENT;  // (1/B)*ln(R/R0)
    steinhart += 1.0 / (NTC_NOMINAL_TEMP_C + KELVIN_OFFSET);                    // + 1/T0
    float celsius = 1.0 / steinhart - KELVIN_OFFSET;              // назад у °C
    return celsius;
}
