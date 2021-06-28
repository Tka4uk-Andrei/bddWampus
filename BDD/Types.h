#pragma once

#include "bdd.h"

typedef unsigned int uint;
typedef long long lint;


/// <summary>
///     ���������, ����������� ���������� ��������, ��������� �� �������
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
///     ��������� � �������������, ���� ������� �����, ��� ���������� ����������
/// </summary>
struct Directions
{
    bdd n; // �����
    bdd e; // �����
    bdd s; // ��
    bdd w; // ������
};


/// <summary>
///     ������� ����������� ���������� �������� ���������� ����������� ��� ����������� n �����
/// </summary>
/// <param name="n"> -- ���������� �����, ������� ���������� ������������ </param>
/// <returns> ���������� �������� ���������� ����������� ��� ������������� </returns>
uint log_2(int n)
{
    uint val = 1;
    while ((1 << val) < n)
    {
        ++val;
    }
    return val;
}

