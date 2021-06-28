#pragma once

#include "bdd.h"

typedef unsigned int uint;
typedef long long lint;


/// <summary>
///     Структура, описывающая переменные действия, ЗАВИСЯЩИЕ ОТ ВРЕМЕНИ
/// </summary>
struct TimeDependentActions
{
    bdd Forward;
    bdd TurnRight;
    bdd TurnLeft;

    bdd Grab;
    bdd Climb;
    bdd Shoot;
};


/// <summary>
///     Структура с направлениями, куда смотрит агент, для символьных вычислений
/// </summary>
struct Directions
{
    bdd n; // Север
    bdd e; // Запад
    bdd s; // Юг
    bdd w; // Восток
};


/// <summary>
///     Функция вычисляющая количество двоичных переменных необходимых для кодирования n чисел
/// </summary>
/// <param name="n"> -- количество чисел, которые необходимо закодировать </param>
/// <returns> количество двоичных переменных необходимых для представления </returns>
uint log_2(int n)
{
    uint val = 1;
    while ((1 << val) < n)
    {
        ++val;
    }
    return val;
}

