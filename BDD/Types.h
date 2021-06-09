#pragma once

#include "bdd.h"

typedef unsigned int uint;
typedef long long lint;

// Структура, описывающая переменные действия, ЗАВИСЯЩИЕ ОТ ВРЕМЕНИ
struct TimeDependentActions
{
    bdd Forward;
    bdd TurnRight;
    bdd TurnLeft;

    bdd Grab;
    bdd Climb;
    bdd Shoot;
};


// Структура с направлениями, куда смотрит агент, для символьных вычислений
struct Directions
{
    bdd n; // Север
    bdd e; // Запад
    bdd s; // Юг
    bdd w; // Восток
};
