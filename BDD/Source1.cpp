#pragma comment(lib,"bdd.lib")
#pragma warning (disable : 26444)
#include"bdd.h"
#include<fstream>
#include <iostream>
#include <vector>
#include <cmath>
#include <time.h>
#include <map>

///////////////////////////////// СЭТКАУНТ НЕ МЕНЯЕТСЯ ПРИ ДОБАВЛЕНИИ ИЛИ УДАЛЕНИИ ОГРАНИЧЕНИЙ ЧТО ДЕЛАТЬ?? ПЕРЕДЕЛАТЬ ////////////////////////////////////////

using namespace std;

ofstream out;

//N_DEPEND_ON_CELL*N_COLUMN*N_ROW+N_NOT_DEPEND_ON_CELL+18
#define N_DEPEND_ON_CELL 9
#define N_NOT_DEPEND_ON_CELL 18
#define N_COLUMN 1
#define N_ROW 1
#define N N_COLUMN*N_ROW
#define N_VAR 50 // число булевых переменных (220?)
void fun(char* varset, int size); //функция, используемая для вывода решений
void print(); // Печать в файл


//переменные Stench и Breeze [2], так как нет формул, которые связывают предыдущее и текущее

//переменные восприятия среды
bdd Stench[2];
bdd Breeze[2];
bdd Scream[2];

// переменные состояния среды, СТАТИЧНЫЕ, ОТ ВРЕМЕНИ НЕ ЗАВИСЯТ
bdd B[N_COLUMN*N_ROW]; //ветер
bdd P[N_COLUMN*N_ROW]; //яма
bdd S[N_COLUMN*N_ROW]; //запах
bdd W[N_COLUMN*N_ROW]; //вампус


//переменные состояния среды, ЗАВИСЯТ ОТ ВРЕМЕНИ
bdd HaveArrow[2];//иметь стрелу 
bdd HaveArrow_next[2]; //иметь стрелу в следующем состоянии

bdd WumpusAlive[2]; //Вампус жив 
bdd WumpusAlive_next[2];

bdd HaveGold[2]; //иметь золото 
bdd HaveGold_next[2];

bdd ClimbedOut[2]; //вылезти из пещеры 
bdd ClimbedOut_next[2];

//переменные положения агента, ЗАВИСЯТ ОТ ВРЕМЕНИ
bdd L[N]; //нахождение агента
bdd L_next[N];

bdd North[2]; //смотреть на север
bdd North_next[2];

bdd West[2];
bdd West_next[2];

bdd South[2];
bdd South_next[2];

bdd East[2];
bdd East_next[2];

bdd V[N]; //посещена агентом
bdd V_next[N];

bdd OK[N]; //безопасная клетка
bdd OK_next[N];

//переменные действия, ЗАВИСЯТ ОТ ВРЕМЕНИ
bdd Forward[2];
bdd Forward_next[2];

bdd TurnRight[2];
bdd TurnRight_next[2];

bdd TurnLeft[2];
bdd TurnLeft_next[2];

bdd Grab[2];
bdd Grab_next[2];

bdd Climb[2];
bdd Climb_next[2];

bdd Shoot[2];
bdd Shoot_next[2];

//сначала проинициализировать статику, потом динамику
int main() {
	int countvar = 0;
	bdd_init(10000000, 1000000);//Выделяем память для 1000000 строк таблицы и КЭШ размером 100000
	bdd_setvarnum(N_VAR); //задаем количество булевых переменных

	bdd task = bddtrue; //Решение. Изначально true. Здесь будет находиться база
	
	//////////////////////////////////// С Т А Т И К А ////////////////////////////////////////////////

	//для Вампуса
	for (int i = 0; i < N; i++)
	{
		W[i] = bdd_ithvar(countvar);
		countvar++;
		//(i == 8) ? W[i] = bdd_ithvar(i) : W[i] = bdd_nithvar(i); //нитвара не будет, в таске я просто умножаю на отрицание W[i]
	}

	//для запаха
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

	//для Stench

	Stench[0] = bdd_ithvar(countvar); //отрицание переменной запаха
	countvar++;
	Stench[1] = bdd_ithvar(countvar); //положительное значение переменной запаха
	countvar++;

	//для Breeze

	Breeze[0] = bdd_ithvar(countvar); //отрицание переменной ветра
	countvar++;
	Breeze[1] = bdd_ithvar(countvar);  //положительное значение переменной ветра
	countvar++;

	//для Scream
	Scream[0] = bdd_ithvar(countvar);
	countvar++;
	Scream[1] = bdd_ithvar(countvar);
	countvar++;


	///////////////////////////////////////////////Д И Н А М И К А//////////////////////////////////////////////////////

	//для нахождения агента в клетке
	for (int i=0; i<N; i++)
	{
		L[i] = bdd_ithvar(countvar);
		countvar++;
	}

	//для посещение клетки агентом
	for (int i=0; i<N; i++)
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
	HaveArrow[0] = bdd_ithvar(countvar); //нет стрелы
	countvar++;
	HaveArrow[1] = bdd_ithvar(countvar); //есть стрела
	countvar++;

	//для того что Вампус жив
	WumpusAlive[0] = bdd_ithvar(countvar);
	countvar++;
	WumpusAlive[1] = bdd_ithvar(countvar);
	countvar++;

	//для направлений
	East[0] = bdd_ithvar(countvar);
	countvar++;
	East[1] = bdd_ithvar(countvar);
	countvar++;

	South[0] = bdd_ithvar(countvar);
	countvar++;
	South[1] = bdd_ithvar(countvar);
	countvar++;

	West[0] = bdd_ithvar(countvar);
	countvar++;
	West[1] = bdd_ithvar(countvar);
	countvar++;

	North[0] = bdd_ithvar(countvar);
	countvar++;
	North[1] = bdd_ithvar(countvar);
	countvar++;

	//для действий (форвард, тёрнлефт, тёрнрайт)
	Forward[0] = bdd_ithvar(countvar);
	countvar++;
	Forward[1] = bdd_ithvar(countvar);
	countvar++;

	TurnLeft[0] = bdd_ithvar(countvar);
	countvar++;
	TurnLeft[1] = bdd_ithvar(countvar);
	countvar++;

	TurnRight[0] = bdd_ithvar(countvar);
	countvar++;
	TurnRight[1] = bdd_ithvar(countvar);
	countvar++;

	Grab[0] = bdd_ithvar(countvar);
	countvar++;
	Grab[1] = bdd_ithvar(countvar);
	countvar++;

	Climb[0] = bdd_ithvar(countvar);
	countvar++;
	Climb[1] = bdd_ithvar(countvar);
	countvar++;

	Shoot[0] = bdd_ithvar(countvar);
	countvar++;
	Shoot[1] = bdd_ithvar(countvar);
	countvar++;

	//для состояний агента (золото, вылез ли из пещеры?)

	ClimbedOut[0] = bdd_ithvar(countvar);
	countvar++;
	ClimbedOut[1] = bdd_ithvar(countvar);
	countvar++;

	HaveGold[0] = bdd_ithvar(countvar);
	countvar++;
	HaveGold[1] = bdd_ithvar(countvar);
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

	HaveArrow_next[0] = bdd_ithvar(countvar); //нет стрелы
	countvar++;
	HaveArrow_next[1] = bdd_ithvar(countvar); //есть стрела
	countvar++;

	//для того что Вампус жив
	WumpusAlive_next[0] = bdd_ithvar(countvar);
	countvar++;
	WumpusAlive_next[1] = bdd_ithvar(countvar);
	countvar++;

	//для направлений
	East_next[0] = bdd_ithvar(countvar);
	countvar++;
	East_next[1] = bdd_ithvar(countvar);
	countvar++;

	South_next[0] = bdd_ithvar(countvar);
	countvar++;
	South_next[1] = bdd_ithvar(countvar);
	countvar++;

	West_next[0] = bdd_ithvar(countvar);
	countvar++;
	West_next[1] = bdd_ithvar(countvar);
	countvar++;

	North_next[0] = bdd_ithvar(countvar);
	countvar++;
	North_next[1] = bdd_ithvar(countvar);
	countvar++;

	//для действий (форвард, тёрнлефт, тёрнрайт)
	Forward_next[0] = bdd_ithvar(countvar);
	countvar++;
	Forward_next[1] = bdd_ithvar(countvar);
	countvar++;

	TurnLeft_next[0] = bdd_ithvar(countvar);
	countvar++;
	TurnLeft_next[1] = bdd_ithvar(countvar);
	countvar++;

	TurnRight_next[0] = bdd_ithvar(countvar);
	countvar++;
	TurnRight_next[1] = bdd_ithvar(countvar);
	countvar++;

	Grab_next[0] = bdd_ithvar(countvar);
	countvar++;
	Grab_next[1] = bdd_ithvar(countvar);
	countvar++;

	Climb_next[0] = bdd_ithvar(countvar);
	countvar++;
	Climb_next[1] = bdd_ithvar(countvar);
	countvar++;

	Shoot_next[0] = bdd_ithvar(countvar);
	countvar++;
	Shoot_next[1] = bdd_ithvar(countvar);
	countvar++;

	//для состояний агента (золото, вылез ли из пещеры?)

	ClimbedOut_next[0] = bdd_ithvar(countvar);
	countvar++;
	ClimbedOut_next[1] = bdd_ithvar(countvar);
	countvar++;

	HaveGold_next[0] = bdd_ithvar(countvar);
	countvar++;
	HaveGold_next[1] = bdd_ithvar(countvar);
	countvar++;


	//////////////////////////////////////////////////////////////// Г Е Н Е Р А Ц И Я   К А Р Т Ы //////////////////////////////////////////////////////////////////


	std::vector <int> real_cave(16);

	int* temp_pit = new int [3];//для ям
	int temp_w;
	int temp_g;
	int* index_pit = new int [3];
	int index_w;
	int index_g;
	srand(time(NULL));
	int k = (int)(0.2 * N);
	int nums[15] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15 };

	/////////////////////////////////////////////////////////// Г Е Н Е Р А Ц И Я   Я М,   В А М П У С А,   З О Л О Т А ///////////////////////////////////////////////////////////////////////////////////
	index_pit[0] = rand() % (N - 1);
	real_cave[index_pit[0]] = 3;
	for (int i = 1; i < k; i++)
	{

		for (int j = 1; j < k; j++)
		{
			index_pit[j] = index_pit[i-1];
			if (index_pit[i] != index_pit[j])
			{
				temp_pit[i] = nums[index_pit[i]];
			}
			else index_pit[i] = rand() % (N - 1);
		}
			real_cave[temp_pit[i]] = 3;
	}
	index_w = rand() % (N - 1);
	for (int i = 0; i < k; i++)
	{
		if (index_w == index_pit[i])
			index_w = rand() % (N - 1);
		else
		{
			temp_w = nums[index_w];
			real_cave[temp_w] = 2;
		}
	}
	index_g = rand() % (N - 1);
	for (int i = 0; i < k+1; i++)
	{
		if (index_g == index_pit[i] || index_g == index_w)
			index_g = rand() % (N - 1);
		else
		{
			temp_g = nums[index_g];
			real_cave[temp_g] = 4;
		}
	}
	int k1 = 0;
	real_cave[0] = 1;

	cout << " 1 - AGENT \n 2 - WUMPUS \n 3 - PIT \n 4 - GOLD \n 22 - STENCH \n 33 - BREEZE \n ";
	cout << "The real CAVE is:" << "\n";
	
	for (int i = 0; i < N; i++)
	{
		cout << real_cave[i] << "   ";
		k1++;
		if (k1 % N_COLUMN == 0)
			cout << "\n";
	}


	//cout << countvar;


	//добавляем Вампуса в таск (а нужно ли добавлять сюда то, что он в 8ой клетке или нужно добавить только то, что его нет в 1?
	//task &= W[8];

	// НАЧАЛЬНАЯ БЗ АГЕНТА
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

	task &= HaveArrow[1]; //есть стрела
	task &= WumpusAlive[1];

	task &= East[1];
	task &= !West[0];
	task &= !South[0];
	task &= !North[0];

	task &= !Forward[0];
	task &= !TurnLeft[0];
	task &= !TurnRight[0];

	task &= !Grab[0];
	task &= !Shoot[0];
	task &= !Climb[0];

	task &= !ClimbedOut[0];
	task &= !HaveGold[0];

	//БАЗА ЗНАНИЙ (ФОНОВЫЕ ЗНАНИЯ)
	for (int i = 0; i < N; i++) //ПРОВЕРИТЬ (^ или &???)
	{
		if ((i % N_COLUMN == 0) && (i/N_ROW == 0)) //для 0
		{
			task &= (!(B[i] ^ P[i + 1]) & !(B[i] ^ P[i + N_COLUMN]));
			task &= (!(S[i] ^ W[i + 1]) & !(S[i] ^ W[i + N_COLUMN]));
		}
		else if (i % N_COLUMN == N_COLUMN-1) //для 1 и 2
		{
			task &= (!(B[i] ^ P[i - 1]) & !(B[i] ^ P[i + N_COLUMN]) & !(B[i] ^ P[i + 1]));
			task &= (!(S[i] ^ W[i - 1]) & !(S[i] ^ W[i + N_COLUMN]) & !(S[i] ^ W[i + 1]));
		}
		else if (i % N_COLUMN == 0) //для 4 и 8
		{
			task &= (!(B[i] ^ P[i + 1]) & !(B[i] ^ P[i + N_COLUMN]) & !(B[i] ^ P[i - N_COLUMN]));
			task &= (!(S[i] ^ W[i + 1]) & !(S[i] ^ W[i + N_COLUMN]) & !(S[i] ^ W[i - N_COLUMN]));
		}
		else if ((i % N_COLUMN == 0) && (i / N_ROW == N_ROW-1)) //для 12
		{
			task &= (!(B[i] ^ P[i + 1]) & !(B[i] ^ P[i - N_COLUMN]));
			task &= (!(S[i] ^ W[i + 1]) & !(S[i] ^ W[i - N_COLUMN]));
		}
		else if (i / N_ROW == N_ROW - 1) //для 13 и 14
		{
			task &= (!(B[i] ^ P[i + 1]) & !(B[i] ^ P[i - N_COLUMN]) & !(B[i] ^ P[i - 1]));
			task &= (!(S[i] ^ W[i + 1]) & !(S[i] ^ W[i - N_COLUMN]) & !(S[i] ^ W[i - 1]));
		}
		else if ((i / N_ROW == N_ROW - 1) && (i % N_COLUMN == N_COLUMN - 1)) //для 15
		{
			task &= (!(B[i] ^ P[i - 1]) & !(B[i] ^ P[i - N_COLUMN]));
			task &= (!(S[i] ^ W[i - 1]) & !(S[i] ^ W[i - N_COLUMN]));
		}
		else if ((i / N_ROW == 0) && (i % N_COLUMN == N_COLUMN - 1)) // для 3
		{
			task &= (!(B[i] ^ P[i - 1]) & !(B[i] ^ P[i + N_COLUMN]));
			task &= (!(S[i] ^ W[i - 1]) & !(S[i] ^ W[i + N_COLUMN]));
		}
		else if (i % N_COLUMN == N_COLUMN - 1) //для 7 и 11
		{
			task &= (!(B[i] ^ P[i + N_COLUMN]) & !(B[i] ^ P[i - N_COLUMN]) & !(B[i] ^ P[i - 1]));
			task &= (!(S[i] ^ W[i + N_COLUMN]) & !(S[i] ^ W[i - N_COLUMN]) & !(S[i] ^ W[i - 1]));
		}
		else //для 5, 6, 9, 10
		{
			task &= (!(B[i] ^ P[i + N_COLUMN]) & !(B[i] ^ P[i - N_COLUMN]) & !(B[i] ^ P[i - 1]) & !(B[i] ^ P[i + 1]));
			task &= (!(S[i] ^ W[i + N_COLUMN]) & !(S[i] ^ W[i - N_COLUMN]) & !(S[i] ^ W[i - 1]) & !(S[i] ^ W[i + 1]));
		}
	}

	//// ЕСТЬ ХОТЯ БЫ ОДИН ВАМПУС КАК СДЕЛАТЬ?

	//for (int i = 0; i < N; i++) //то что не более одного Вампуса           И С П Р А В И Т Ь 
	//{
	//	for (int j = 0; j < N; j++)
	//	{
	//		if (j != i)
	//			task &= !(W[i] ^ !W[j]);
	//	}
	//}

	task &= !(HaveArrow_next[1] ^ (HaveArrow[0] & !Shoot[0])); //запуталась с нулями и отрицанием.............(ОТРИЦАТЬ 0ой элементы или 1ый?) отрицаю 0й элемент, так как это текущее значение
	task &= !(WumpusAlive_next[1] ^ (WumpusAlive[0] & !Scream[0]));

	//направление агента
	task &= !(North_next[1] ^ ((TurnRight[0] & West[0]) | (TurnLeft[0] & East[0]) | (North[0] & !TurnLeft[0] & !TurnRight[0])));
	task &= !(South_next[1] ^ ((TurnRight[0] & East[0]) | (TurnLeft[0] & West[0]) | (South[0] & !TurnLeft[0] & !TurnRight[0])));
	task &= !(East_next[1] ^ ((TurnRight[0] & North[0]) | (TurnLeft[0] & South[0]) | (East[0] & !TurnLeft[0] & !TurnRight[0])));
	task &= !(West_next[1] ^ ((TurnRight[0] & South[0]) | (TurnLeft[0] & North[0]) | (West[0] & !TurnLeft[0] & !TurnRight[0])));

	task &= !(HaveGold_next[1] ^ (Grab[0] & HaveGold[0]));
	task &= !(ClimbedOut_next[1] ^ (Climb[0] & ClimbedOut[0])); //чтоооооооо кааааааак почемууууу

	for (int i = 0; i < N; i++) //здесь не форвард(1) или не форвард(0)???  И С П Р А В И Т Ь 
	{
		if (i % N_COLUMN == 1)
			task &= !(L_next[i] ^ ((L[i] & !Forward[1]) | (L[i-N_COLUMN] & North[0] & Forward[0]) | (L[i+1] & West[0] & Forward[0]) | (L[i+N_COLUMN] & South[0] & Forward[0])));
		else if (i % N_COLUMN == 0)
			task &= !(L_next[i] ^ ((L[i] & !Forward[0]) | (L[i - N_COLUMN] & North[0] & Forward[0]) | (L[i - 1] & East[0] & Forward[0]) | (L[i + N_COLUMN] & South[0] & Forward[0])));
		else
			task &= !(L_next[i] ^ ((L[i] & !Forward[0]) | (L[i - N_COLUMN] & North[0] & Forward[0]) | (L[i - 1] & East[0] & Forward[0]) | (L[i + 1] & West[0] & Forward[0]) | (L[i + N_COLUMN] & South[0] & Forward[0])));

		pair<bdd, bdd> for_swap;
		pair<bdd, bdd> swaped;
		for_swap.first = L[i];
		for_swap.second = L_next[i];
		swaped.first = for_swap.second;
		swaped.second = for_swap.first;
		L[i] = swaped.first;
	}

	for (int i = 0; i < N; i++)
	{
		task &= !(V_next[i] ^ (V[i]|L[i]));
	}

	for (int i = 0; i < N; i++) //переменные со стороны среды с учетом восприятия
	{
		task &= (L[i] >> (!(Breeze[0] ^ B[i])));
		task &= (L[i] >> (!(Stench[0] ^ S[i])));
		task &= L[i] >> V[i];
		task &= !(OK[i] ^ (!P[i] & !(W[i] & WumpusAlive[0])));
	}

	//взаимодействие действий и переменных (здесь тоже (отрицаю от 0 или от 1?))

	task &= (Shoot[0] >> HaveArrow[0]);

	task &= (Forward[0] >> (!Shoot[0] & !TurnLeft[0] & !TurnRight[0] & !Grab[0] & !Climb[0]));
	task &= (Shoot[0] >> (!Forward[0] & !TurnLeft[0] & !TurnRight[0] & !Grab[0] & !Climb[0]));
	task &= (TurnLeft[0] >> (!Shoot[0] & !Forward[0] & !TurnRight[0] & !Grab[0] & !Climb[0]));
	task &= (TurnRight[0] >> (!Shoot[0] & !TurnLeft[0] & !Forward[0] & !Grab[0] & !Climb[0]));
	task &= (Grab[0] >> (!Shoot[0] & !TurnLeft[0] & !Forward[0] & !TurnRight[0] & !Climb[0]));
	task &= (Climb[0] >> (!Shoot[0] & !TurnLeft[0] & !Forward[0] & !Grab[0] & !TurnRight[0]));


	out.open("out.txt");
	unsigned int satcount = (int)bdd_satcount(task);// количество решений
	out << satcount << " solutions:\n" << endl;
	//if (satcount) bdd_allsat(task, fun);
	out.close();
	bdd_done();
	system("pause");
}

char var[N_VAR];

//void print() {
//	for (int i = 0; i < N_COLUMN; i++) {
//		out << i << ": ";
//		for (unsigned j = 0; j < N_ROW; j++) {
//			int J = i * N_COLUMN * N_ROW + j * N_ROW;
//			int num = 0;
//			for (unsigned k = 0; k < N_ROW; k++) num += (unsigned)
//				(var[J + k] << k);
//			out << num << ' ';
//		}
//		out << endl;
//	}
//	out << endl;
//}
void build(char* varset, int n, int I)
{
	if (I == n - 1) {
		if (varset[I] >= 0) {
			var[I] = varset[I];
			//print();
			return;
		}
		var[I] = 0;
		//print();
		var[I] = 1;
		//print();
		return;
	}
	if (varset[I] >= 0) {
		var[I] = varset[I];
		build(varset, n, I + 1);
		return;
	}
	var[I] = 0;
	build(varset, n, I + 1);
	var[I] = 1;
	build(varset, n, I + 1);
}
void fun(char* varset, int size) {
	build(varset, size, 0);
}