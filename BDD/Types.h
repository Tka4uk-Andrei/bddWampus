#pragma once

#include "bdd.h"

typedef unsigned int uint;
typedef long long lint;

// ���������, ����������� ���������� ��������, ��������� �� �������
struct TimeDependentActions
{
    bdd Forward;
    bdd TurnRight;
    bdd TurnLeft;

    bdd Grab;
    bdd Climb;
    bdd Shoot;
};


// ��������� � �������������, ���� ������� �����, ��� ���������� ����������
struct Directions
{
    bdd n; // �����
    bdd e; // �����
    bdd s; // ��
    bdd w; // ������
};
