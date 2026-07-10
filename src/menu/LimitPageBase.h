// ============================================================
//  LimitPageBase.h
//  Спільна база для LimitSelectPage і LimitValueEditPage (FR-3) —
//  обидві показують нижню/верхню межу каналу в однаковому форматі
//  (drawRow(): курсор "> ...<" на обраному полі), кожна лише по-
//  своєму вирішує, ЯКЕ поле зараз обране і що з ним робити по
//  кнопці. render() тут спільний і фінальний — просто малює через
//  drawAll(), яку визначає кожен нащадок.
// ============================================================
#pragma once

#include "MenuPage.h"
#include "../ChannelTemp.h"

class LimitPageBase : public MenuPage {
public:
    enum class Field { Low, High };

protected:
    ChannelTemp& channel;   // чиї межі показуємо/редагуємо
    Field field;            // яке поле зараз обране (курсор або те, що редагується)

    explicit LimitPageBase(ChannelTemp& channel_);

    // Значення фіксованою шириною 5 символів (вкладається "-20.0".."105.0"
    // з FR-1) — без цього коротше число лишало б "привида" від
    // попереднього довшого при перемальовуванні.
    void printField(LiquidCrystal_I2C& lcd, float v);

    // Один рядок межі: курсор "> " зліва + підпис + значення + "<" справа
    // від обраного поля — обране значення виглядає "затиснутим" між > і <.
    void drawRow(LiquidCrystal_I2C& lcd, int row, const char* label, float v, bool selected);

    // Конкретний вигляд сторінки (які значення й яке поле обране) —
    // визначає кожен нащадок; render() лише викликає це.
    virtual void drawAll(LiquidCrystal_I2C& lcd) = 0;

public:
    void render(LiquidCrystal_I2C& lcd) override final;
};
