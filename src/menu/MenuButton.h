#pragma once

// Спільний тип вводу для всіх сторінок меню.
// Назва MenuButton (не Button) — щоб не зіткнутися з класом Button
// із бібліотеки EncButton (глобальний простір імен).
enum class MenuButton { None, Up, Down, Ok, Back };
