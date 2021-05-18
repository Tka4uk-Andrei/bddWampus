#pragma comment(lib,"bdd.lib")
#pragma warning (disable : 26444)

#include"bdd.h"
#include<fstream>
#include <iostream>
#include <vector>
#include <cmath>
#include <time.h>
#include <map>
#include <stack>
#include <chrono>

#include "WorldMap.h"

#define DIRECTIONS

using namespace std;

ofstream out;

//N_DEPEND_ON_CELL*N_COLUMN*N_ROW+N_NOT_DEPEND_ON_CELL+18
#define N_DEPEND_ON_CELL 9
#define N_NOT_DEPEND_ON_CELL 18

#define N_FUNCTIONS_AT_CELL (3 + 14*4)
#define N_FUNCTIONS_IN_FIELD (4 + 3*4) // до percept здесь было 4+3*2
#define N_VAR 207

int find_unvisited(bdd task, int current_cell); //прохожусь по соседям клетки, смотри те, которые еще не проверены, отправляю их на проверку
bdd ask_and_send_percept(vector<vector <int>> Enviroment, int current_cell); // из i-ой клетки достает восприятие (percept)
int check_for_safety(bdd task, int current_cell); //проверяет клетку на безопасность
int Enviroment(bdd task, int current_cell); //сочетает две функции: поиск соседей и проверку на безопасность
void find_path(bdd relation, bdd first_state, bdd* q, bdd* qq, bdd wished_state, vector<int> answer, vector<bdd> visited, bdd& direction);
int move(int cell, string action, bdd& direction);
void fun(char* varset, int size); //функция, используемая для вывода решений
void print(); // Печать в файл

//переменные восприятия среды
bdd Stench;
bdd Breeze;
bdd Scream;

// переменные состояния среды, СТАТИЧНЫЕ, ОТ ВРЕМЕНИ НЕ ЗАВИСЯТ
bdd B[N]; //ветер
bdd P[N]; //яма
bdd S[N]; //запах
bdd W[N]; //вампус
bdd G[N]; //золото

// направления, куда смотрит агент, для символьных вычислений
bdd n; // Север
bdd e; // Запад
bdd s; // Юг
bdd w; // Восток

//переменные состояния среды, ЗАВИСЯТ ОТ ВРЕМЕНИ
bdd HaveArrow;//иметь стрелу
bdd HaveArrow_next; //иметь стрелу в следующем состоянии

bdd WumpusAlive; //Вампус жив
bdd WumpusAlive_next;

bdd HaveGold; //иметь золото
bdd HaveGold_next;

bdd ClimbedOut; //вылезти из пещеры
bdd ClimbedOut_next;

//переменные положения агента, ЗАВИСЯТ ОТ ВРЕМЕНИ
bdd L[N]; //нахождение агента
bdd L_next[N];

bdd North; //смотреть на север
bdd North_next;

bdd West;
bdd West_next;

bdd South;
bdd South_next;

bdd East;
bdd East_next;

bdd V[N]; //посещена агентом
bdd V_next[N];

bdd OK[N]; //безопасная клетка
bdd OK_next[N];

//переменные действия, ЗАВИСЯТ ОТ ВРЕМЕНИ
bdd Forward;
bdd Forward_next;

bdd TurnRight;
bdd TurnRight_next;

bdd TurnLeft;
bdd TurnLeft_next;

bdd Grab;
bdd Grab_next;

bdd Climb;
bdd Climb_next;

bdd Shoot;
bdd Shoot_next;

bool checked_cells[N] = { true, false, false, false,
                          false, false, false, false,
                          false, false, false, false,
                          false, false, false, false };  //массив проверенных клеток

bool not_safe_cells[N]; //массив небезопасных клеток

bool unknown_cells[N]; //пока не можем говорить, безопасные или нет

bool safe_cells[N] = { true, false, false, false,
                       false, false, false, false,
                       false, false, false, false, 
                       false, false, false, false};

stack <int> cells[N]; //стек для хранения предыдущей клетки

// флаги для символьных вычислений
bool flag = false;
bool to_stop_recursion;

// Определение соседей относительно клетки cell
vector<int> neighbours(int cell)
{
    vector <int> neighbours;

    // если ячейка не в крайнем левом столбце
    if (cell % N_COLUMN != 0)
    {
        neighbours.push_back(cell - 1);
    }
    // если ячейка не в крайнем правом столбце
    if ((cell + 1) % N_COLUMN != 0)
    {
        neighbours.push_back(cell + 1);
    }
    // если ячейка не на самой нижней строке
    if (cell < N_COLUMN * (N_ROW - 1))
    {
        neighbours.push_back(cell + N_COLUMN);
    }
    // если ячейка не на самой верхней строке
    if (cell >= N_COLUMN)
    {
        neighbours.push_back(cell - N_COLUMN);
    }

    return neighbours;
}

stack <bdd> plan;
stack <string> str_plan;

int main()
{
    // Замечание. Сначала инициализируем статику, потом динамику

    // Инициализация Buddy
    int countvar = 0;             // Сквозной счетчик для bdd
    bdd_init(10000000, 10000000); // Выделяем память для 1000000 строк таблицы и кэш размером 100000
    bdd_setvarnum(N_VAR);         // Задаем количество булевых переменных

    cout << " 1 - Agent\n"
        << " 2 - Wumpus\n"
        << " 3 - PIT\n"
        << " 4 - GOLD\n"
        << " 22 - Stench\n"
        << " 33 - Breeze" << endl;

    bdd task = bddtrue; //Решение. Изначально true. Здесь будет находиться база
    bdd movements = bddtrue;

    //////////////////////////////////// С Т А Т И К А ////////////////////////////////////////////////
    {
        // для Вампуса
        for (int i = 0; i < N; i++)
        {
            W[i] = bdd_ithvar(countvar);
            countvar++;
        }

        // для запаха
        for (int i = 0; i < N; i++)
        {
            S[i] = bdd_ithvar(countvar);
            countvar++;
        }

        //для ям
        for (int i = 0; i < N; i++)
        {
            P[i] = bdd_ithvar(countvar);
            countvar++;
        }

        //для ветра
        for (int i = 0; i < N; i++)
        {
            B[i] = bdd_ithvar(countvar);
            countvar++;
        }

        // Для золота
        for (int i = 0; i < N; i++)
        {
            G[i] = bdd_ithvar(countvar);
            countvar++;
        }

        //для Stench

        Stench = bdd_ithvar(countvar); // переменной запаха
        countvar++;

        //для Breeze

        Breeze = bdd_ithvar(countvar); // переменной ветра
        countvar++;

        //для Scream
        Scream = bdd_ithvar(countvar);
        countvar++;
    }

	///////////////////////////////////////////////Д И Н А М И К А//////////////////////////////////////////////////////

	//для нахождения агента в клетке
	for (int i = 0; i < N; i++)
	{
		L[i] = bdd_ithvar(countvar);
		countvar++;
	}

	//для посещение клетки агентом
	for (int i = 0; i < N; i++)
	{
		V[i] = bdd_ithvar(countvar);
		countvar++;
	}

	//для безопасной клетки
	for (int i = 0; i < N; i++)
	{
		OK[i] = bdd_ithvar(countvar);
		countvar++;
	}

	//для стрелы
	HaveArrow = bdd_ithvar(countvar); //есть стрела в текущей клетке
	countvar++;

	//для того что Вампус жив
	WumpusAlive = bdd_ithvar(countvar);
	countvar++;

	//для направлений
	East = bdd_ithvar(countvar);
	countvar++;

	South = bdd_ithvar(countvar);
	countvar++;

	West = bdd_ithvar(countvar);
	countvar++;

	North = bdd_ithvar(countvar);
	countvar++;

	//для действий (форвард, тёрнлефт, тёрнрайт)
	Forward = bdd_ithvar(countvar);
	countvar++;

	TurnLeft = bdd_ithvar(countvar);
	countvar++;

	TurnRight = bdd_ithvar(countvar);
	countvar++;

	Grab = bdd_ithvar(countvar);
	countvar++;

	Climb = bdd_ithvar(countvar);
	countvar++;

	Shoot = bdd_ithvar(countvar);
	countvar++;

	//для состояний агента (золото, вылез ли из пещеры?)

	ClimbedOut = bdd_ithvar(countvar);
	countvar++;

	HaveGold = bdd_ithvar(countvar);
	countvar++;

	////////////////////////////////////////ПЕРЕМЕННЫЕ ДЛЯ СЛЕДУЮЩЕГО СОСТОЯНИЯ///////////////////////////////////////

	for (int i = 0; i < N; i++)
	{
		L_next[i] = bdd_ithvar(countvar);
		countvar++;
	}

	for (int i = 0; i < N; i++)
	{
		V_next[i] = bdd_ithvar(countvar);
		countvar++;
	}

	for (int i = 0; i < N; i++)
	{
		OK_next[i] = bdd_ithvar(countvar);
		countvar++;
	}

	HaveArrow_next = bdd_ithvar(countvar); //нет стрелы
	countvar++;

	//для того что Вампус жив
	WumpusAlive_next = bdd_ithvar(countvar);
	countvar++;

	//для направлений
	East_next = bdd_ithvar(countvar);
	countvar++;

	South_next = bdd_ithvar(countvar);
	countvar++;

	West_next = bdd_ithvar(countvar);
	countvar++;

	North_next = bdd_ithvar(countvar);
	countvar++;

	//для действий (форвард, тёрнлефт, тёрнрайт)
	Forward_next = bdd_ithvar(countvar);
	countvar++;

	TurnLeft_next = bdd_ithvar(countvar);
	countvar++;

	TurnRight_next = bdd_ithvar(countvar);
	countvar++;

	Grab_next = bdd_ithvar(countvar);
	countvar++;

	Climb_next = bdd_ithvar(countvar);
	countvar++;

	Shoot_next = bdd_ithvar(countvar);
	countvar++;

	//для состояний агента (золото, вылез ли из пещеры?)

	ClimbedOut_next = bdd_ithvar(countvar);
	countvar++;

	HaveGold_next = bdd_ithvar(countvar);
	countvar++;




	// НАЧАЛЬНАЯ БЗ АГЕНТА                                          ПРОВЕРЕНО
	task &= L[0];
	task &= V[0];
	task &= OK[0];

	for (int i = 1; i < N; i++)
	{
		task &= !L[i];
		task &= !V[i];
	}

	task &= !P[0];
	task &= !W[0];

	task &= HaveArrow; //есть стрела
	task &= WumpusAlive;

	task &= East;
	task &= !West;
	task &= !South;
	task &= !North;

	task &= !Forward;
	task &= !TurnLeft;
	task &= !TurnRight;

	task &= !Grab;
	task &= !Shoot;
	task &= !Climb;

	task &= !ClimbedOut;
	task &= !HaveGold;

	/////////////////////////////////////////////// БАЗА ЗНАНИЙ НЕ ЗАВИСЯЩАЯ ОТ ВРЕМЕНИ //////////////////////////////////////////// 

	for (int i = 0; i < N; i++)
	{

		if (i == 0) //для начальной, нулевой клетки (0)
		{
			bdd temp = bddtrue;

			temp &= !B[i] ^ (P[i + 1] | P[i + N_COLUMN]);
			temp &= !P[i] ^ (B[i + 1] & B[i + N_COLUMN]);
			temp &= !S[i] ^ ((!W[i + 1] & W[i + N_COLUMN]) | (W[i + 1] & !W[i + N_COLUMN])); // либо | между скобками, либо ^ (исключающее ИЛИ)
			temp &= !W[i] ^ (S[i] & S[i + 1]);

			task &= temp;
		}
		else if (i % N_COLUMN == 0 && i != N - N_COLUMN && i != 0) //для 4 и 8
		{
			bdd temp = bddtrue;

			temp &= !B[i] ^ (P[i + 1] | P[i + N_COLUMN] | P[i - N_COLUMN]);
			temp &= !P[i] ^ (B[i + 1] & B[i + N_COLUMN] & B[i - N_COLUMN]);
			temp &= !S[i] ^ ((W[i + 1] & !W[i + N_COLUMN] & !W[i - N_COLUMN]) | (!W[i + 1] & W[i + N_COLUMN] & !W[i - N_COLUMN]) | (!W[i + 1] & !W[i + N_COLUMN] & W[i - N_COLUMN]));
			temp &= !W[i] ^ (S[i] & S[i + N_COLUMN] & S[i - N_COLUMN]);

			task &= temp;
		}

		else if (i % N_COLUMN == 0 && i == N - N_COLUMN) //для 12
		{

			bdd temp = bddtrue;

			temp &= !B[i] ^ (P[i + 1] | P[i - N_COLUMN]);
			temp &= !P[i] ^ (B[i + 1] & B[i - N_COLUMN]);
			temp &= !S[i] ^ ((W[i + 1] & !W[i - N_COLUMN]) | (!W[i + 1] & W[i - N_COLUMN]));
			temp &= !W[i] ^ (S[i + 1] & S[i - N_COLUMN]);

			task &= temp;
		}

		else if (i / N_COLUMN == 0 && i != N_COLUMN - 1 && i != 0) //для 1 и 2
		{

			bdd temp = bddtrue;

			temp &= !B[i] ^ (P[i + 1] | P[i - 1] | P[i + N_COLUMN]);
			temp &= !P[i] ^ (B[i + 1] & B[i - 1] & B[i + N_COLUMN]);
			temp &= !S[i] ^ ((W[i + 1] & !W[i - 1] & !W[i + N_COLUMN]) | (!W[i + 1] & W[i - 1] & !W[i + N_COLUMN]) | (!W[i + 1] & !W[i - 1] & W[i + N_COLUMN]));
			temp &= !W[i] ^ (S[i + 1] & S[i - 1] & S[i + N_COLUMN]);

			task &= temp;
		}

		else if (i / N_COLUMN == 0 && i == N_COLUMN - 1) //для 3
		{

			bdd temp = bddtrue;

			temp &= !B[i] ^ (P[i - 1] | P[i + N_COLUMN]);
			temp &= !P[i] ^ (B[i - 1] & B[i + N_COLUMN]);
			temp &= !S[i] ^ ((W[i - 1] & !W[i + N_COLUMN]) | (!W[i - 1] & W[i + N_COLUMN]));
			temp &= !W[i] ^ (S[i - 1] & S[i + N_COLUMN]);

			task &= temp;
		}

		else if (i % N_COLUMN == N_COLUMN - 1 && i != N - 1) //для 7 и 11
		{

			bdd temp = bddtrue;

			temp &= !B[i] ^ (P[i - 1] | P[i - N_COLUMN] | P[i + N_COLUMN]);
			temp &= !P[i] ^ (B[i - 1] & B[i - N_COLUMN] & B[i + N_COLUMN]);
			temp &= !S[i] ^ ((W[i - 1] & !W[i - N_COLUMN] & !W[i + N_COLUMN]) | (!W[i - 1] & W[i - N_COLUMN] & !W[i + N_COLUMN]) | (!W[i - 1] & !W[i - N_COLUMN] & W[i + N_COLUMN]));
			temp &= !W[i] ^ (S[i - 1] & S[i - N_COLUMN] & S[i + N_COLUMN]);

			task &= temp;
		}

		else if (i % N_COLUMN == N_COLUMN - 1 && i == N - 1) //для 15
		{

			bdd temp = bddtrue;

			temp &= !B[i] ^ (P[i - 1] | P[i - N_COLUMN]);
			temp &= !P[i] ^ (B[i - 1] & B[i - N_COLUMN]);
			temp &= !S[i] ^ ((W[i - 1] & !W[i - N_COLUMN]) | (!W[i - 1] & W[i - N_COLUMN]));
			temp &= !W[i] ^ (S[i - 1] & S[i - N_COLUMN]);

			task &= temp;
		}

		else if (i / N_ROW == N_ROW - 1 && i != N - N_ROW && i != N - 1) //для 13 и 14
		{

			bdd temp = bddtrue;

			temp &= !B[i] ^ (P[i - 1] | P[i + 1] | P[i - N_COLUMN]);
			temp &= !P[i] ^ (B[i - 1] & B[i + 1] & B[i - N_COLUMN]);
			temp &= !S[i] ^ ((W[i - 1] & !W[i + 1] & !W[i - N_COLUMN]) | (!W[i - 1] & W[i + 1] & !W[i - N_COLUMN]) | (!W[i - 1] & !W[i + 1] & W[i - N_COLUMN]));
			temp &= !W[i] ^ (S[i - 1] & S[i + 1] & S[i - N_COLUMN]);

			task &= temp;
		}
		else if (i % N_COLUMN != 0 && i % N_COLUMN != N_COLUMN - 1 && i / N_COLUMN != 0 && i / N_COLUMN != N_COLUMN - 1) //для тех, что в серединке (5,6,9,10)
		{

			bdd temp = bddtrue;

			temp &= !B[i] ^ (P[i - 1] | P[i + 1] | P[i - N_COLUMN] | P[i + N_COLUMN]);
			temp &= !P[i] ^ (B[i - 1] & B[i + 1] & B[i - N_COLUMN] & B[i + N_COLUMN]);
			temp &= !S[i] ^ ((W[i - 1] & !W[i + 1] & !W[i - N_COLUMN] & !W[i + N_COLUMN]) | (!W[i - 1] & W[i + 1] & !W[i - N_COLUMN] & !W[i + N_COLUMN]) | (!W[i - 1] & !W[i + 1] & W[i - N_COLUMN] & !W[i + N_COLUMN]) | (!W[i + 1] & !W[i - 1] & !W[i - N_COLUMN] & W[i + N_COLUMN]));
			temp &= !W[i] ^ (S[i - 1] & S[i + 1] & S[i - N_COLUMN] & S[i + N_COLUMN]);

			task &= temp;
		}
	}

//for (int i = 0; i < N; i++) //для карты 3х3
//{
//
//	if (i == 0) //для начальной, нулевой клетки (0)
//	{
//		bdd temp = bddtrue;
//
//		temp &= !B[i] ^ (P[i + 1] | P[i + N_COLUMN]);
//		temp &= !P[i] ^ (B[i + 1] & B[i + N_COLUMN]);
//		temp &= !S[i] ^ ((!W[i + 1] & W[i + N_COLUMN]) | (W[i + 1] & !W[i + N_COLUMN]));
//		temp &= !W[i] ^ (S[i] & S[i + 1]);
//
//		task &= temp;
//	}
//	else if ((i / N_COLUMN == 0 && i != N_COLUMN - 1 && i != 0)) //для 1
//	{
//
//		bdd temp = bddtrue;
//
//		temp &= !B[i] ^ (P[i + 1] | P[i - 1] | P[i + N_COLUMN]);
//		temp &= !P[i] ^ (B[i + 1] & B[i - 1] & B[i + N_COLUMN]);
//		temp &= !S[i] ^ ((W[i + 1] & !W[i - 1] & !W[i + N_COLUMN]) | (!W[i + 1] & W[i - 1] & !W[i + N_COLUMN]) | (!W[i + 1] & !W[i - 1] & W[i + N_COLUMN]));
//		temp &= !W[i] ^ (S[i + 1] & S[i - 1] & S[i + N_COLUMN]);
//
//		task &= temp;
//	}
//	else if (i == N_ROW - 1) //для 2
//	{
//
//		bdd temp = bddtrue;
//
//		temp &= !B[i] ^ (P[i - 1] | P[i + N_COLUMN]);
//		temp &= !P[i] ^ (B[i - 1] & B[i + N_COLUMN]);
//		temp &= !S[i] ^ ((!W[i - 1] & W[i + N_COLUMN]) | (W[i - 1] & !W[i + N_COLUMN]));
//		temp &= !W[i] ^ (S[i - 1] & S[i + N_COLUMN]);
//
//		task &= temp;
//	}
//	else if (i == N_COLUMN) //для 3
//	{
//
//		bdd temp = bddtrue;
//
//		temp &= !B[i] ^ (P[i + 1] | P[i - N_COLUMN] | P[i + N_COLUMN]);
//		temp &= !P[i] ^ (B[i + 1] & B[i - N_COLUMN] & B[i + N_COLUMN]);
//		temp &= !S[i] ^ ((W[i + 1] & !W[i - N_COLUMN] & !W[i + N_COLUMN]) | (!W[i + 1] & W[i - N_COLUMN] & !W[i + N_COLUMN]) | (!W[i + 1] & !W[i - N_COLUMN] & W[i + N_COLUMN]));
//		temp &= !W[i] ^ (S[i + 1] & S[i - N_COLUMN] & S[i + N_COLUMN]);
//
//		task &= temp;
//	}
//	else if (i % N_COLUMN == 1 && i / N_COLUMN == 1 && i != 0) //для 4
//	{
//
//				bdd temp = bddtrue;
//
//				temp &= !B[i] ^ (P[i - 1] | P[i + 1] | P[i - N_COLUMN] | P[i + N_COLUMN]);
//				temp &= !P[i] ^ (B[i - 1] & B[i + 1] & B[i - N_COLUMN] & B[i + N_COLUMN]);
//				temp &= !S[i] ^ ((W[i - 1] & !W[i + 1] & !W[i - N_COLUMN] & !W[i + N_COLUMN]) | (!W[i - 1] & W[i + 1] & !W[i - N_COLUMN] & !W[i + N_COLUMN]) | (!W[i - 1] & !W[i + 1] & W[i - N_COLUMN] & !W[i + N_COLUMN]) | (!W[i + 1] & !W[i - 1] & !W[i - N_COLUMN] & W[i + N_COLUMN]));
//				temp &= !W[i] ^ (S[i - 1] & S[i + 1] & S[i - N_COLUMN] & S[i + N_COLUMN]);
//
//				task &= temp;
//	}
//	else if (i % N_COLUMN == N_COLUMN - 1 && i != N - 1) //для 5
//	{
//
//		bdd temp = bddtrue;
//
//		temp &= !B[i] ^ (P[i + N_COLUMN] | P[i - 1] | P[i - N_COLUMN]);
//		temp &= !P[i] ^ (B[i + N_COLUMN] & B[i - 1] & B[i - N_COLUMN]);
//		temp &= !S[i] ^ ((W[i + N_COLUMN] & !W[i - 1] & !W[i - N_COLUMN]) | (!W[i + N_COLUMN] & W[i - 1] & !W[i - N_COLUMN]) | (!W[i + N_COLUMN] & !W[i - 1] & W[i - N_COLUMN]));
//		temp &= !W[i] ^ (S[i + N_COLUMN] & S[i - 1] & S[i - N_COLUMN]);
//
//		task &= temp;
//	}
//	else if (i % N_COLUMN == 0 && i / N_COLUMN == N_COLUMN - 1) //для 6
//	{
//
//		bdd temp = bddtrue;
//
//		temp &= !B[i] ^ (P[i + 1] | P[i - N_COLUMN]);
//		temp &= !P[i] ^ (B[i + 1] & B[i - N_COLUMN]);
//		temp &= !S[i] ^ ((!W[i + 1] & W[i - N_COLUMN]) | (W[i + 1] & !W[i - N_COLUMN]));
//		temp &= !W[i] ^ (S[i + 1] & S[i - N_COLUMN]);
//
//		task &= temp;
//	}
//	else if (i % N_COLUMN == 1 && i / N_COLUMN == N_COLUMN - 1) //для 7
//	{
//
//		bdd temp = bddtrue;
//
//		temp &= !B[i] ^ (P[i + 1] | P[i - 1] | P[i - N_COLUMN]);
//		temp &= !P[i] ^ (B[i + 1] & B[i - 1] & B[i - N_COLUMN]);
//		temp &= !S[i] ^ ((W[i + 1] & !W[i - 1] & !W[i - N_COLUMN]) | (!W[i + 1] & W[i - 1] & !W[i - N_COLUMN]) | (!W[i + 1] & !W[i - 1] & W[i - N_COLUMN]));
//		temp &= !W[i] ^ (S[i + 1] & S[i - 1] & S[i - N_COLUMN]);
//
//		task &= temp;
//	}
//	else if (i == N - 1) //для 8
//	{
//
//		bdd temp = bddtrue;
//
//		temp &= !B[i] ^ (P[i - 1] | P[i - N_COLUMN]);
//		temp &= !P[i] ^ (B[i - 1] & B[i - N_COLUMN]);
//		temp &= !S[i] ^ ((W[i - 1] & !W[i - N_COLUMN]) | (!W[i - 1] & W[i - N_COLUMN]));
//		temp &= !W[i] ^ (S[i - 1] & S[i - N_COLUMN]);
//
//		task &= temp;
//	}
//}

	/////////////////////////////////////////////////////////// ДВЕ ФУНКЦИИ ПРО ВАМПУСА //////////////////////////////////////// 

	bdd temp_w = bddfalse;
	for (int i = 0; i < N; i++)
	{
		temp_w |= W[i];
	}
	task &= temp_w;


	for (int i = 0; i < N; i++) //то что не более одного Вампуса
	{
		for (int j = 0; j < N; j++)
		{
			if (j != i)
			{
				task &= (W[i] >> !W[j]);

			}
		}
	}

	task &= (!HaveArrow_next ^ (HaveArrow & !Shoot));
	task &= (!WumpusAlive_next ^ (WumpusAlive & !Scream));

	//направление агента
	task &= (!North_next ^ ((TurnRight & West) | (TurnLeft & East) | (North & !TurnLeft & !TurnRight)));
	task &= (!South_next ^ ((TurnRight & East) | (TurnLeft & West) | (South & !TurnLeft & !TurnRight)));
	task &= (!East_next ^ ((TurnRight & North) | (TurnLeft & South) | (East & !TurnLeft & !TurnRight)));
	task &= (!West_next ^ ((TurnRight & South) | (TurnLeft & North) | (West & !TurnLeft & !TurnRight)));

	task &= (!HaveGold_next ^ (Grab & HaveGold));
	task &= (!ClimbedOut_next ^ (Climb & ClimbedOut));

	// для изменения местоположения агента, пока не трогала
	for (int i = 0; i < N; i++)
	{
		if (i % N_COLUMN != 0 && i % N_COLUMN != N_COLUMN - 1 && i / N_COLUMN != 0 && i / N_COLUMN != N_COLUMN - 1) //для тех, что в серединке (5,6,9,10)
		{
			task &= !L_next[i] ^ ((L[i] & !Forward) | (L[i - N_COLUMN] & Forward & South) | (L[i + 1] & Forward & West) | (L[i + N_COLUMN] & Forward & North) | (L[i - 1] & Forward & East));
		}
		else if (i % N_COLUMN == 0 && i != N - N_COLUMN && i != 0) //для 4 и 8
		{
			task &= !L_next[i] ^ ((L[i] & !Forward) | (L[i - N_COLUMN] & Forward & South) | (L[i + 1] & Forward & West) | (L[i + N_COLUMN] & Forward & North));
		}
		else if (i / N_ROW == N_ROW - 1 && i != N - N_ROW && i != N - 1) //для 13 и 14
		{
			task &= !L_next[i] ^ ((L[i] & !Forward) | (L[i - N_COLUMN] & Forward & South) | (L[i - 1] & Forward & East) | (L[i + 1] & Forward & West));
		}
		else if (i % N_COLUMN == N_COLUMN - 1 && i == N - 1) //для 15
		{
			task &= !L_next[i] ^ ((L[i] & !Forward) | (L[i - N_COLUMN] & Forward & South) | (L[i - 1] & Forward & East));
		}
		else if (i % N_COLUMN == N_COLUMN - 1 && i != N - 1) //для 7 и 11
		{
			task &= !L_next[i] ^ ((L[i] & !Forward) | (L[i - N_COLUMN] & Forward & South) | (L[i - 1] & Forward & East) | (L[i + N_COLUMN] & Forward & North));
		}
		else if (i / N_COLUMN == 0 && i == N_COLUMN - 1) //для 3
		{
			task &= !L_next[i] ^ ((L[i] & !Forward) | (L[i + N_COLUMN] & Forward & North) | (L[i - 1] & Forward & East));
		}
		else if (i / N_COLUMN == 0 && i != N_COLUMN - 1 && i != 0) //для 1 и 2
		{
			task &= !L_next[i] ^ ((L[i] & !Forward) | (L[i - N_COLUMN] & Forward & South) | (L[i - 1] & Forward & East) | (L[i + 1] & Forward & West));
		}
		else if (i % N_COLUMN == 0 && i == N - N_COLUMN) //для 12
		{
			task &= !L_next[i] ^ ((L[i] & !Forward) | (L[i - N_COLUMN] & Forward & South) | (L[i + 1] & Forward & West));
		}
		if (i == 0) // для 0
		{
			task &= !L_next[i] ^ ((L[i] & !Forward) | (L[i + N_COLUMN] & Forward & North) | (L[i + 1] & Forward & West));
		}
	}

	for (int i = 0; i < N; i++)
	{
		task &= !(V_next[i] ^ (V[i] | L[i]));
	}

	for (int i = 0; i < N; i++) // переменные со стороны среды с учетом восприятия
	{
		task &= (L[i] >> (!Breeze ^ B[i]));
		task &= (L[i] >> (!Stench ^ S[i]));
		task &= L[i] >> V[i];
		task &= ((!OK[i] | !P[i]) & (OK[i] | P[i] | W[i]) & (OK[i] | P[i] | WumpusAlive) & (!OK[i] | !W[i] | !WumpusAlive));
	}

	// взаимодействие действий и переменных

	task &= (Shoot >> HaveArrow);

	task &= (Forward >> (!Shoot & !TurnLeft & !TurnRight & !Grab & !Climb));
	task &= (Shoot >> (!Forward & !TurnLeft & !TurnRight & !Grab & !Climb));
	task &= (TurnLeft >> (!Shoot & !Forward & !TurnRight & !Grab & !Climb));
	task &= (TurnRight >> (!Shoot & !TurnLeft & !Forward & !Grab & !Climb));
	task &= (Grab >> (!Shoot & !TurnLeft & !Forward & !TurnRight & !Climb));
	task &= (Climb >> (!Shoot & !TurnLeft & !Forward & !Grab & !TurnRight));


	task &= (East >> (!West & !South & !North));
	task &= (West >> (!East & !South & !North));
	task &= (South >> (!West & !East & !North));
	task &= (North >> (!West & !East & !South));

	/////////////////////////////////////////// СИМВОЛЬНЫЕ ВЫЧИСЛЕНИЯ ///////////////////////////////////////////////////////////////////

	bdd x[12];
	bdd q[16];  // нештрихованные
	bdd qq[16]; // штрихованные

	for (int i = 0; i < 12; i++)
	{
		x[i] = bdd_ithvar(i);
	}

    q[0] = !x[0] & !x[1] & !x[2] & !x[3]; // 0000
    q[1] = !x[0] & !x[1] & !x[2] & x[3];  // 0001
    q[2] = !x[0] & !x[1] & x[2] & !x[3];  // 0010
    q[3] = !x[0] & !x[1] & x[2] & x[3];   // 0011

    q[4] = !x[0] & x[1] & !x[2] & !x[3]; // 0100
    q[5] = !x[0] & x[1] & !x[2] & x[3];  // 0101
    q[6] = !x[0] & x[1] & x[2] & !x[3];  // 0110
    q[7] = !x[0] & x[1] & x[2] & x[3];   // 0111

    q[8] = x[0] & !x[1] & !x[2] & !x[3]; // 1000
    q[9] = x[0] & !x[1] & !x[2] & x[3];  // 1001
    q[10] = x[0] & !x[1] & x[2] & !x[3]; // 1010
    q[11] = x[0] & !x[1] & x[2] & x[3];  // 1011

    q[12] = x[0] & x[1] & !x[2] & !x[3]; // 1100
    q[13] = x[0] & x[1] & !x[2] & x[3];  // 1101
    q[14] = x[0] & x[1] & x[2] & !x[3];  // 1110
    q[15] = x[0] & x[1] & x[2] & x[3];   // 1111


    qq[0] = !x[4] & !x[5] & !x[6] & !x[7]; // 0000
    qq[1] = !x[4] & !x[5] & !x[6] & x[7];  // 0001
    qq[2] = !x[4] & !x[5] & x[6] & !x[7];  // 0010
    qq[3] = !x[4] & !x[5] & x[6] & x[7];   // 0011

    qq[4] = !x[4] & x[5] & !x[6] & !x[7];  // 0100
    qq[5] = !x[4] & x[5] & !x[6] & x[7];   // 0101
    qq[6] = !x[4] & x[5] & x[6] & !x[7];   // 0110
    qq[7] = !x[4] & x[5] & x[6] & x[7];    // 0111

    qq[8] = x[4] & !x[5] & !x[6] & !x[7]; // 1000
    qq[9] = x[4] & !x[5] & !x[6] & x[7];  // 1001
    qq[10] = x[4] & !x[5] & x[6] & !x[7]; // 1010
    qq[11] = x[4] & !x[5] & x[6] & x[7];  // 1011

    qq[12] = x[4] & x[5] & !x[6] & !x[7]; // 1100
    qq[13] = x[4] & x[5] & !x[6] & x[7];  // 1101
    qq[14] = x[4] & x[5] & x[6] & !x[7];  // 1110
    qq[15] = x[4] & x[5] & x[6] & x[7];   // 1111

    n = !x[8] & !x[9]; // 00
    s = !x[8] & x[9];  // 01
    e = x[8] & !x[9];  // 10
    w = x[8] & x[9];   // 11

    bdd R = q[0] & e & qq[1] | q[0] & s & qq[4] |
        q[1] & w & qq[0] | q[1] & s & qq[5] | q[1] & e & qq[2] |
        q[2] & w & qq[1] | q[2] & s & qq[6] | q[2] & e & qq[3] |
        q[3] & w & qq[2] | q[3] & s & qq[7] |

        q[4] & e & qq[5] | q[4] & s & qq[8] | q[4] & n & qq[0] |
        q[5] & w & qq[4] | q[5] & s & qq[9] | q[5] & n & qq[1] | q[5] & e & qq[6] |
        q[6] & e & qq[7] | q[6] & n & qq[2] | q[6] & w & qq[5] | q[6] & s & qq[10] |
        q[7] & s & qq[11] | q[7] & w & qq[6] | q[7] & n & qq[3] |

        q[8] & e & qq[9] | q[8] & n & qq[4] | q[8] & s & qq[12] |
        q[9] & e & qq[10] | q[9] & w & qq[8] | q[9] & n & qq[5] | q[9] & s & qq[13] |
        q[10] & e & qq[11] | q[10] & w & qq[9] | q[10] & n & qq[6] | q[10] & s & qq[14] |
        q[11] & w & qq[10] | q[11] & s & qq[15] | q[11] & n & qq[7] |

        q[12] & e & qq[13] | q[12] & n & qq[8] |
        q[13] & e & qq[14] | q[13] & w & qq[12] | q[13] & n & qq[9] |
        q[14] & e & qq[15] | q[14] & w & qq[13] | q[14] & n & qq[10] |
        q[15] & w & qq[14] | q[15] & n & qq[11];

	// начинаем всегда с 0 клетки
	int current_cell = 0;
	int cell_to_go = 0;

	vector<int> cells_visited_by_agent = { 0 };
	bool reverse_cells[N] = { false, false, false, false,
						false, false, false, false,
						false, false, false, false,
						false, false, false, false };

    // Вывожу карту
	bool gold_flag = false;
	int k1 = 0;
	for (unsigned p = 0; p < real_cave2.size(); p++)
	{
		cout << "( ";
		for (unsigned q = 0; q < real_cave2[p].size(); q++)
		{
			cout << real_cave2[p][q];
			cout << " ";
		}
		cout << ")";
		k1++;
		if (k1 % 4 == 0)
			cout << "\n";
	}

	bdd relation;
	vector <bdd> visited = { qq[current_cell] };
	vector <int> answer;
	bdd direction = e;
	stack <string> new_plan;
	stack <int> previous_cell;

	auto start = std::chrono::high_resolution_clock::now();
	cout << "Current cell is 0" << endl;
	do { 

		bdd percept = ask_and_send_percept(real_cave2, current_cell);
		if ((percept &= !G[current_cell]) == bddfalse)
		{
			gold_flag = true;
			cout << "In cell " << current_cell << " is Gold! " << endl;
			cout << endl;
			cout << "Now agent is going back to the exit" << endl;
			cout << endl;
			bdd new_relation;
			while (current_cell != 0)
			{
				int count = 0;
				previous_cell.push(current_cell);
				reverse_cells[current_cell] = true;
				new_relation = R & q[current_cell];
				vector<int> neighb = neighbours(current_cell);
				for (int i : neighb)
				{
					count++;
					if (safe_cells[i] == true && reverse_cells[i] == false)
					{
						new_relation &= qq[i];
						cell_to_go = i;
						find_path(R, q[current_cell], q, qq, qq[cell_to_go], answer, visited, direction);
						to_stop_recursion = false;
						int size = str_plan.size();
						for (int j = 0; j < size; j++)
						{
							new_plan.push(str_plan.top());
							str_plan.pop();
						}

						for (int j = 0; j < size; j++)
						{
							if (!new_plan.empty())
							{
								string action = new_plan.top();
								current_cell = move(current_cell, action, direction);
								cout << "action done by an agent - " << action << endl;
								new_plan.pop();
							}
						}
						current_cell = cell_to_go;
						cout << "Current cell is " << current_cell << endl;
						count--;
						break;
					}
				}
				if (neighb.size() == count)
				{
					current_cell = previous_cell.top();
					previous_cell.pop();
					current_cell = previous_cell.top();
					previous_cell.pop();
					cout << "Current cell is " << current_cell << endl;
				}
			}
			break;
		}

		task &= percept;

		cell_to_go = Enviroment(task, current_cell);

		bdd current_relation = R & q[current_cell];

		current_relation &= qq[cell_to_go];

		relation |= current_relation;

		find_path(current_relation, q[current_cell], q, qq, qq[cell_to_go], answer, visited, direction);
		to_stop_recursion = false;
		int size = str_plan.size();
		for (int i = 0; i < size; i++)
		{
			new_plan.push(str_plan.top());
			str_plan.pop();
		}

		for (int i = 0; i < size; i++)
		{
			if (!new_plan.empty())
			{
				string action = new_plan.top();
				current_cell = move(current_cell, action, direction);
				cout << "action done by an agent - " << action << endl;
				new_plan.pop();
			}
		}
		cells_visited_by_agent.push_back(current_cell);
		cout << "Current cell is " << current_cell << endl;

	} while (gold_flag != true);
	auto diff = std::chrono::high_resolution_clock::now() - start; // разница 
	auto nsec = std::chrono::duration_cast<std::chrono::nanoseconds>(diff);
	cout << "It took: " << nsec.count() << " nanoseconds" << endl;

	bdd_done();
	system("pause");
}

char var[N_VAR];

int check_for_safety(bdd task, int current_cell) //проверка клетки на безопасность
{
	int cell_to_go_next;
	if ((task & !OK[current_cell]) == bddfalse) //если она безопасна
	{
		cell_to_go_next = current_cell; //говорю, что хочу пойти в эту клетку
		checked_cells[current_cell] = true; //говорю, что проверила эту клетку
		safe_cells[current_cell] = true; //говорю, что эта клетка безопасна
		flag = true;
	}
	else if ((task & OK[current_cell]) == bddfalse) //опасна
	{
		not_safe_cells[current_cell] = true; //говорим, что это клетка опасна
		checked_cells[current_cell] = true;
		cell_to_go_next = cells->top();
		if ((task &= !P[current_cell]) == bddfalse)
			cout << "In cell " << current_cell << " is Pit!" << endl;
		else if ((task &= !W[current_cell]) == bddfalse)
			cout << "In cell " << current_cell << " is Wumpus!" << endl;
	}
	else //если неизвстна
	{
		unknown_cells[current_cell] = true; //говорим, что небезопасна
		//checked_cells[current_cell] = true; //добавляем в массив проверенных
		cell_to_go_next = cells->top();
	}
	return cell_to_go_next;
}

int find_unvisited(bdd task, int current_cell) //прохожусь по соседям клетки, смотри те, которые еще не проверены, отправляю их на проверку
{
	vector <int> neighbours_of_cell = neighbours(current_cell);

	for (int i = 0; i < neighbours_of_cell.size(); i++)
	{
		int check_if_safe = neighbours_of_cell[i];
		if (checked_cells[check_if_safe] == false)
		{
			cells->push(current_cell);
			current_cell = check_if_safe;
			break;
		}
	}
	return current_cell;
}

bdd ask_and_send_percept(vector<vector <int>> Enviroment, int current_cell) // в текущей клетке беру percept
{
	{
		bdd percept = bddtrue;
		for (int i = 0; i < Enviroment[current_cell].size(); i++)
		{
			if (Enviroment[current_cell][i] == 22)
				percept &= S[current_cell];
			else if (Enviroment[current_cell][i] == 33)
				percept &= B[current_cell];
			else if (Enviroment[current_cell][i] == 4)
				percept &= G[current_cell];
			else percept &= !B[current_cell] & !S[current_cell] & !G[current_cell];
		}
		return percept;
	}
}

int Enviroment(bdd task, int current_cell)
{
	int cell_to_check;
	int got_unsafe_cell;
	int count = 0;

	cell_to_check = find_unvisited(task, current_cell); //ближайшая непосещенная клетка, которую можно проверить на безопасность
	cell_to_check = check_for_safety(task, cell_to_check); //проверка на безопасность

	if (cell_to_check == current_cell)
	{
		vector<int> neighb = neighbours(current_cell);
		for (int i = 0; i < neighb.size(); i++)
			if (checked_cells[neighb[i]] == true)
			{
				count++;
			}
		if (count == neighb.size())
		{
			for (int i = 0; i < neighb.size(); i++)
			{
				if (safe_cells[neighb[i]] == true)
					return got_unsafe_cell = neighb[i];
			}
		}
		else if (unknown_cells[find_unvisited(task, current_cell)] == true)
		{
			for (int i = 0; i < neighb.size(); i++)
			{
				if (unknown_cells[neighb[i]] == false && checked_cells[neighb[i]] == false)
				{
					return cell_to_check = check_for_safety(task, neighb[i]);
				}
			}
		}
		if (cell_to_check == current_cell)
		{
			for (int i = 0; i < neighb.size(); i++)
			{
				if (safe_cells[neighb[i]] == true)
					return got_unsafe_cell = neighb[i];
			}
		}
	}
	else if (flag == true)
	{
		checked_cells[cell_to_check] = true;
		return cell_to_check;
	}
}

int move(int cell, string action, bdd &direction) //функция для передвижения
{
	if (direction == n)
	{
		if (action == "forward")
			cell = cell - N_ROW;
		else if (action == "turnLeft")
			direction = w;
		else if (action == "turnRight")
			direction = e;
	}
	else if (direction == s)
	{
		if (action == "forward")
			cell = cell + N_ROW;
		else if (action == "turnLeft")
			direction = e;
		else if (action == "turnRight")
			direction = w;
	}
	else if (direction == e)
	{
		if (action == "forward")
			cell = cell + 1;
		else if (action == "turnLeft")
			direction = n;
		else if (action == "turnRight")
			direction = s;
	}
	else if (direction == w)
	{
		if (action == "forward")
			cell = cell -1;
		else if (action == "turnLeft")
			direction = s;
		else if (action == "turnRight")
			direction = n;
	}
	return cell;
}

void find_path(bdd relation, bdd first_state, bdd* q, bdd* qq, bdd wished_state, vector<int> answer, vector<bdd> visited, bdd &direction)
{
	bool flag = false;
	bool dostijima = false;
	int dir;
	while (!flag && !dostijima)
	{
		bdd X_pr = first_state;
		bdd X_R = first_state & relation & wished_state;

		for (int i = 0; i < 16; i++)
		{

			if (to_stop_recursion == true)
				break;
			if ((X_R & qq[i]) != bddfalse && qq[i] == wished_state)
			{
				answer.push_back(i);
				dostijima = true;
				flag = true;
				visited.push_back(qq[i]);
				if (direction == n)
					dir = 1;
				else if (direction == s)
					dir = 2;
				else if (direction == e)
					dir = 3;
				else if (direction == w)
					dir = 4;
				switch (dir)
				{
				case 1: //север
				{
					if ((X_R & qq[i] & direction) != bddfalse)
					{
						str_plan.push("forward");
						plan.push(Forward_next);
					}

					else if ((X_R & qq[i] & direction) == bddfalse)
					{
						if ((X_R & qq[i] & e) != bddfalse)
						{
							str_plan.push("turnRight");
							plan.push(TurnRight_next);
							str_plan.push("forward");
							plan.push(Forward_next);
							//direction = e;
						}
						else if ((X_R & qq[i] & w) != bddfalse)
						{
							str_plan.push("turnLeft");
							plan.push(TurnLeft_next);
							str_plan.push("forward");
							plan.push(Forward_next);
							//direction = w;
						}
						else if ((X_R & qq[i] & s) != bddfalse)
						{
							str_plan.push("turnLeft");
							plan.push(TurnLeft_next);
							str_plan.push("turnLeft");
							plan.push(TurnLeft_next);
							str_plan.push("forward");
							plan.push(Forward_next);
							//direction = s;
						}
					}
				} break;
				case 2: //юг
				{
					if ((X_R & qq[i] & direction) != bddfalse)
					{
						str_plan.push("forward");
						plan.push(Forward_next);
					}
					else if ((X_R & qq[i] & direction) == bddfalse)
					{
						if ((X_R & qq[i] & e) != bddfalse)
						{
							str_plan.push("turnLeft");
							plan.push(TurnLeft_next);
							str_plan.push("forward");
							plan.push(Forward_next);
							//direction = e;
						}
						else if ((X_R & qq[i] & w) != bddfalse)
						{
							str_plan.push("turnRight");
							plan.push(TurnRight_next);
							str_plan.push("forward");
							plan.push(Forward_next);
							//direction = w;
						}
						else if ((X_R & qq[i] & n) != bddfalse)
						{
							str_plan.push("turnLeft");
							plan.push(TurnLeft_next);
							str_plan.push("turnLeft");
							plan.push(TurnLeft_next);
							str_plan.push("forward");
							plan.push(Forward_next);
							//direction = n;
						}
					}
				} break;
				case 3: //восток
				{
					if ((X_R & qq[i] & direction) != bddfalse)
					{
						str_plan.push("forward");
						plan.push(Forward_next);
					}
					else if ((X_R & qq[i] & direction) == bddfalse)
					{
						if ((X_R & qq[i] & s) != bddfalse)
						{
							str_plan.push("turnRight");
							plan.push(TurnRight_next);
							str_plan.push("forward");
							plan.push(Forward_next);
							//direction = s;
						}
						else if ((X_R & qq[i] & n) != bddfalse)
						{
							str_plan.push("turnLeft");
							plan.push(TurnLeft_next);
							str_plan.push("forward");
							plan.push(Forward_next);
							//direction = n;
						}
						else if ((X_R & qq[i] & w) != bddfalse)
						{
							str_plan.push("turnLeft");
							plan.push(TurnLeft_next);
							str_plan.push("turnLeft");
							plan.push(TurnLeft_next);
							str_plan.push("forward");
							plan.push(Forward_next);
							//direction = w;
						}
					}
				} break;
				case 4: //запад
				{
					if ((X_R & qq[i] & direction) != bddfalse)
					{
						str_plan.push("forward");
						plan.push(Forward_next);
					}
					else if ((X_R & qq[i] & direction) == bddfalse)
					{
						if ((X_R & qq[i] & s) != bddfalse)
						{
							str_plan.push("turnLeft");
							plan.push(TurnLeft_next);
							str_plan.push("forward");
							plan.push(Forward_next);
							//direction = s;
						}
						else if ((X_R & qq[i] & n) != bddfalse)
						{
							str_plan.push("turnRight");
							plan.push(TurnRight_next);
							str_plan.push("forward");
							plan.push(Forward_next);
							//direction = n;
						}
						else if ((X_R & qq[i] & e) != bddfalse)
						{
							str_plan.push("turnLeft");
							plan.push(TurnLeft_next);
							str_plan.push("turnLeft");
							plan.push(TurnLeft_next);
							str_plan.push("forward");
							plan.push(Forward_next);
							//direction = e;
						}
					}
				}break;
				}
				//out.open("Route.txt", std::ios::app);
				//for (int k = 0; k < answer.size(); k++)
				//{
				//	cout << answer[k];
				//	out << answer[k];
				//}
				//out << endl;
				//out.close();
				//cout << endl;
				//break;
			}
			else if ((X_R & qq[i] & first_state) != bddfalse)
			{
				int j = 0;
				bool cflag = false;
				while (j < visited.size() && !cflag)
				{
					if (qq[i] == visited[j])
					{
						cflag = true;
					}
					j++;
				}
				if (!cflag)
				{
					if (direction == n)
						dir = 1;
					else if (direction == s)
						dir = 2;
					else if (direction == e)
						dir = 3;
					else if (direction == w)
						dir = 4;
					switch (dir)
					{
					case 1: //север
					{
						if ((X_R & qq[i] & direction) != bddfalse)
						{
							str_plan.push("forward");
							plan.push(Forward_next);
						}

						else if ((X_R & qq[i] & direction) == bddfalse)
						{
							if ((X_R & qq[i] & e) != bddfalse)
							{
								str_plan.push("turnRight");
								plan.push(TurnRight_next);
								str_plan.push("forward");
								plan.push(Forward_next);
								//direction = e;
							}
							else if ((X_R & qq[i] & w) != bddfalse)
							{
								str_plan.push("turnLeft");
								plan.push(TurnLeft_next);
								str_plan.push("forward");
								plan.push(Forward_next);
								//direction = w;
							}
							else if ((X_R & qq[i] & s) != bddfalse)
							{
								str_plan.push("turnLeft");
								plan.push(TurnLeft_next);
								str_plan.push("turnLeft");
								plan.push(TurnLeft_next);
								str_plan.push("forward");
								plan.push(Forward_next);
								//direction = s;
							}
						}
					} break;
					case 2: //юг
					{
						if ((X_R & qq[i] & direction) != bddfalse)
						{
							str_plan.push("forward");
							plan.push(Forward_next);
						}
						else if ((X_R & qq[i] & direction) == bddfalse)
						{
							if ((X_R & qq[i] & e) != bddfalse)
							{
								str_plan.push("turnLeft");
								plan.push(TurnLeft_next);
								str_plan.push("forward");
								plan.push(Forward_next);
								//direction = e;
							}
							else if ((X_R & qq[i] & w) != bddfalse)
							{
								str_plan.push("turnRight");
								plan.push(TurnRight_next);
								str_plan.push("forward");
								plan.push(Forward_next);
								//direction = w;
							}
							else if ((X_R & qq[i] & n) != bddfalse)
							{
								str_plan.push("turnLeft");
								plan.push(TurnLeft_next);
								str_plan.push("turnLeft");
								plan.push(TurnLeft_next);
								str_plan.push("forward");
								plan.push(Forward_next);
								//direction = n;
							}
						}
					} break;
					case 3: //восток
					{
						if ((X_R & qq[i] & direction) != bddfalse)
						{
							str_plan.push("forward");
							plan.push(Forward_next);
						}
						else if ((X_R & qq[i] & direction) == bddfalse)
						{
							if ((X_R & qq[i] & s) != bddfalse)
							{
								str_plan.push("turnRight");
								plan.push(TurnRight_next);
								str_plan.push("forward");
								plan.push(Forward_next);
								//direction = s;
							}
							else if ((X_R & qq[i] & n) != bddfalse)
							{
								str_plan.push("turnLeft");
								plan.push(TurnLeft_next);
								str_plan.push("forward");
								plan.push(Forward_next);
								//direction = n;
							}
							else if ((X_R & qq[i] & w) != bddfalse)
							{
								str_plan.push("turnLeft");
								plan.push(TurnLeft_next);
								str_plan.push("turnLeft");
								plan.push(TurnLeft_next);
								str_plan.push("forward");
								plan.push(Forward_next);
								//direction = w;
							}
						}
					} break;
					case 4: //запад
					{
						if ((X_R & qq[i] & direction) != bddfalse)
						{
							str_plan.push("forward");
							plan.push(Forward_next);
						}
						else if ((X_R & qq[i] & direction) == bddfalse)
						{
							if ((X_R & qq[i] & s) != bddfalse)
							{
								str_plan.push("turnLeft");
								plan.push(TurnLeft_next);
								str_plan.push("forward");
								plan.push(Forward_next);
								//direction = s;
							}
							else if ((X_R & qq[i] & n) != bddfalse)
							{
								str_plan.push("turnRight");
								plan.push(TurnRight_next);
								str_plan.push("forward");
								plan.push(Forward_next);
								//direction = n;
							}
							else if ((X_R & qq[i] & e) != bddfalse)
							{
								str_plan.push("turnLeft");
								plan.push(TurnLeft_next);
								str_plan.push("turnLeft");
								plan.push(TurnLeft_next);
								str_plan.push("forward");
								plan.push(Forward_next);
								//direction = e;
							}
						}
					}break;
					}
					for (int j = 0; j < 9; j++)
					{
						if ((relation & q[i] & qq[j]) != bddfalse)
						{
							bool kflag = false;
							int k = 0;
							while (k < visited.size() && !kflag)
							{
								if (qq[j] == visited[k])
								{
									kflag = true;
								}
								k++;
							}
							if (!kflag)
							{
								visited.push_back(qq[i]);
								answer.push_back(i);
								find_path(relation, q[i], q, qq, wished_state, answer, visited, direction);
								break;
							}
						}
					}
				}
			}
		}
		if (X_pr == first_state)
		{
			flag = true;
			to_stop_recursion = true;
		}
		else X_pr = first_state;
	}
}

// не использую эти функции
//void print()
//{
//	for (unsigned i = 0; i < N; i++)
//	{
//		out << i << ": ";
//		int num = 0;
//		num += (unsigned)(var[i]);
//		out << num << " ";
//		out << endl;
//	}
//	out << endl;
//}
//void build(char* varset, int n, int I)
//{
//	if (I == n - 1) {
//		if (varset[I] >= 0) {
//			var[I] = varset[I];
//			print();
//			return;
//		}
//		var[I] = 0;
//		print();
//		var[I] = 1;
//		print();
//		return;
//	}
//	if (varset[I] >= 0) {
//		var[I] = varset[I];
//		build(varset, n, I + 1);
//		return;
//	}
//	var[I] = 0;
//	build(varset, n, I + 1);
//	var[I] = 1;
//	build(varset, n, I + 1);
//}
//void fun(char* varset, int size) {
//	build(varset, size, 0);
//}