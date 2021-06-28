#pragma comment(lib,"bdd.lib")
#pragma warning (disable : 26444)

#include "bdd.h"
#include <fstream>
#include <iostream>
#include <vector>
#include <cmath>
#include <time.h>
#include <map>
#include <stack>
#include <chrono>
#include <iomanip>

#include "WorldMap.h"
#include "Types.h"

#include "PathFinder.h"

using namespace std;

ofstream out;

// TODO добавить readme
// TODO добавить класс для работы с bdd. Через него должна происходить инициализация переменных (обращение к ithvar()).
// TODO подумать над взаимодействием поворотов и напралений (может быть их можно как-то связать с bdd и логическим выводом?)
// TODO добавить модуль проверяющий правильность поведения агента. По логам мы определяем может ли агент совершить то или иное действие
// TODO разделить логирование случая нахождения золота и случая, когдпа безопасный путь найти нельзя

constexpr int DEPEND_ON_NODE_VAR_COUNT = 11; // TODO показать из каких значений состоит
constexpr int INDEPEND_FROM_NODE_VAR_COUNT = 30; // TODO показать из каких значений состоит

//#define N_FUNCTIONS_AT_CELL (3 + 14*4)
//#define N_FUNCTIONS_IN_FIELD (4 + 3*4) // до percept здесь было 4+3*2 (???)

constexpr int NUMS_FOR_DIR = 2;

Map mapInfo;

/// <summary>
///     прохожусь по соседям клетки, смотри те, которые еще не проверены, отправляю их на проверку
/// </summary>
/// <param name="task"> -- Текущая база знаний </param>
/// <param name="current_cell"> -- Номер ячейкий, относительно которой идёт поиск </param>
/// <returns> Номер одной из непосещённых ячеек </returns>
int find_unvisited(bdd task, int current_cell);

/// <summary>
///     Получение восприятия (percept) из указанной клетки
/// </summary>
/// <param name="Enviroment"> -- Карта мира </param>
/// <param name="current_cell"> -- Номер текущей ячейки </param>
/// <returns></returns>
bdd ask_and_send_percept(vector<vector <Node>> Enviroment, int current_cell);

/// <summary>
///     Проверка клетки на безопасность
/// </summary>
/// <param name="task"> -- Текущая база знаний </param>
/// <param name="current_cell"> -- Текущий номер ячейки </param>
/// <returns></returns>
int check_for_safety(bdd task, int current_cell, ostream& out);

/// <summary>
///     Поиск безопасных соседних клеток
/// </summary>
/// <param name="task"> -- Текущая база знаний </param>
/// <param name="current_cell"> --  </param>
/// <param name="out"> -- Поток вывода для логов </param>
/// <returns> Номер безопасной клетки </returns>
int Enviroment(bdd task, int current_cell, ostream& out);

/// <summary>
///     Функция поиска пути (неточно)
/// </summary>
/// <param name="plan"> -- Стек содержащий последовательность действий для достижения цели </param>
/// <param name="str_plan"> -- Стек содержащий последовательность действий для достижения цели в текстовой форме </param>
/// <param name="relation"> -- Описание отношений перехода между узлами </param>
/// <param name="first_state"> -- Узел из которого требуется построить путь (обычная вершина) </param>
/// <param name="q"> -- Вектор обычных вершин (набор вершин в текущий момент времени) </param>
/// <param name="qq"> -- Вектор штрифованных вершин (набор вершин в следующий момент времени) </param>
/// <param name="wished_state"> -- Узел в который хочется прийти (штрихованная вершина) </param>
/// <param name="answer"> -- Набор вершин по которым надо совершить переход </param>
/// <param name="visited"> -- Вектор посещённых вершин (неточно) </param>
/// <param name="direction"> -- Направление куда смотрит агент в текущий момент времени </param>
/// <param name="actionsNext"> -- Описание действий для перемещения </param>
/// <param name="directions"> -- Направления движения </param>
void find_path(stack <bdd> &plan, stack <string> &str_plan,
               bdd relation, bdd first_state, vector<bdd>& q, vector<bdd>& qq,
               bdd wished_state, vector<int> answer, vector<bdd> visited, bdd& direction,
               TimeDependentActions &actionsNext,
               Directions &directions);

/// <summary>
///     Совершение агентом действий на карте
/// </summary>
/// <param name="cell"> -- Номер текущей ячейки </param>
/// <param name="action"> -- Название действия </param>
/// <param name="direction"> -- Текущее направление </param>
/// <param name="directions"> -- Направления движения </param>
/// <returns> Номер ячейки после совершения действия </returns>
int move(int cell, string action, bdd& direction, Directions& dir);

/// <summary>
///     Запуск агента
/// </summary>
/// <param name="filePath"> -- Путь до файла с картой </param>
/// <param name="isAstarUse"> -- При поиске обратного пути использовать A* </param>
/// <param name="out"> -- Поток для вывода логов </param>
/// <param name="isFullRun"> -- Флаг показывающий, полный запуск агента или только проверка алгоритма поиска пути </param>
/// <returns> Время работы в микросекундах </returns>
lint runAgent(string filePath, bool isAstarUse, ostream& out, bool isFullRun, heuristicDist hFun);

//переменные восприятия среды
bdd Stench; // ощущение агентом 
bdd Breeze; // ощущение агентом ветерка
bdd Scream; // ???

// переменные состояния среды, СТАТИЧНЫЕ, ОТ ВРЕМЕНИ НЕ ЗАВИСЯТ
vector<bdd> B; //ветер
vector<bdd> P; //яма
vector<bdd> S; //запах
vector<bdd> W; //вампус
vector<bdd> G; //золото

//переменные состояния среды, ЗАВИСЯЩИЕ ОТ ВРЕМЕНИ
#pragma region Time-dependent environment state variables

bdd HaveArrow;//иметь стрелу
bdd HaveArrow_next; //иметь стрелу в следующем состоянии

bdd WumpusAlive; //Вампус жив
bdd WumpusAlive_next;

bdd HaveGold; //иметь золото
bdd HaveGold_next;

bdd ClimbedOut; //вылезти из пещеры
bdd ClimbedOut_next;

#pragma endregion

//переменные положения агента, ЗАВИСЯЩИЕ ОТ ВРЕМЕНИ
#pragma region Time-dependent agent position variables

vector<bdd> L;      // текущее положение агента
vector<bdd> L_next; // положение агента в следующий момент времени

bdd North;      // агент смотрит на север
bdd North_next;

bdd West;       // агент смотрит на запад
bdd West_next;

bdd South;      // агент смотрит на юг
bdd South_next;

bdd East;      // агент смотрит на восток
bdd East_next;

vector<bdd> V;       // посещённые агентом клетки
vector<bdd> V_next;

vector<bdd> OK;      // безопасные клетки
vector<bdd> OK_next;

#pragma endregion 

//переменные действия, ЗАВИСЯЩИЕ ОТ ВРЕМЕНИ
#pragma region Time-dependent action variables

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

#pragma endregion 

vector<bool> checked_cells;  // массив проверенных клеток
//vector<bool> not_safe_cells; // массив небезопасных клеток
//vector<bool> unknown_cells;  // пока не можем говорить, безопасные или нет
vector<bool> safe_cells;     // массив безопасных клеток

vector<bool> not_safe_cells; //массив небезопасных клеток (он нужен???)
vector<bool> unknown_cells; //пока не можем говорить, безопасные или нет (он нужен???)

stack<int> cells; //стек для хранения предыдущей клетки

// флаги для символьных вычислений
bool flag = false;
bool to_stop_recursion;

int main()
{
    bool aStarOn = true;
    bool fullRun = true;

    constexpr uint TEST_COUNT = 20;
    const string TEST_FOLDER = "../WampusWorldGenerator/";
    const string TEST_RESULTS_FOLDER = "testResults/";

    // количество карт для каждой папки
    vector<uint> mapCounts = { /*10, 10, 10, 10, 10,*/
                              /* 20, 20,*/ /*20, 20, 20, 20,*/
                               30, /*30, 30,*/ 30, 30, 30, 30,
                               40, /*40, 40,*/ 40, 40, 40, 40, 40,
                               50, /*50, 50, 50,*/ 50, /*50,*/ 50, 50,
                               50, /*50, 50, 50,*/ 50, /*50,*/ 50, 50, 50,
                               50, /*50, 50, 50,*/ 50, /*50,*/ 50, /*50,*/ 50};
    // перечисление папок с 
    vector<string> mapFolders = { /*"4x4p0",   "4x4p1",   "4x4p2",   "4x4p3",   "4x4p5",*/
                                  /*"5x5p0",   "5x5p1",*/   /*"5x5p2",   "5x5p3",   "5x5p5",   "5x5p8",*/
                                  "6x6p0",   /*"6x6p1",   "6x6p2",*/   "6x6p3",   "6x6p5",   "6x6p8",   "6x6p13",
                                  "7x7p0",   /*"7x7p1",   "7x7p2",*/   "7x7p3",   "7x7p5",   "7x7p8",   "7x7p13",   "7x7p21",
                                  "8x8p0",   /*"8x8p1",   "8x8p2",   "8x8p3",*/   "8x8p5",  /* "8x8p8",*/   "8x8p13",   "8x8p21",
                                  "9x9p0",   /*"9x9p1",   "9x9p2",   "9x9p3",*/   "9x9p5",  /* "9x9p8",*/   "9x9p13",   "9x9p21",   "9x9p34",
                                  "10x10p0", /*"10x10p1", "10x10p2", "10x10p3",*/ "10x10p5", /*"10x10p8",*/ "10x10p13", /*"10x10p21",*/ "10x10p34"};

    for (int k = 0; k < mapFolders.size(); ++k )
    {
        //ofstream oldRes;
        //oldRes.open(testResFolder + "old_" + mapFolders[k] + ".txt");

        ofstream new1Res;
        new1Res.open(TEST_RESULTS_FOLDER + "new1_" + mapFolders[k] + ".txt");

        ofstream new2Res;
        new2Res.open(TEST_RESULTS_FOLDER + "new2_" + mapFolders[k] + ".txt");

        ofstream new3Res;
        new3Res.open(TEST_RESULTS_FOLDER + "new3_" + mapFolders[k] + ".txt");

        ofstream cmpRes;
        cmpRes.open(TEST_RESULTS_FOLDER + "cmp_" + mapFolders[k] + ".txt");

        lint oldAlgoTimeSum = 0;
        lint new1AlgoTimeSum = 0;
        lint new2AlgoTimeSum = 0;
        lint new3AlgoTimeSum = 0;

        // Цикл по картам одного типа
        for (uint i = 0; i < mapCounts[k]; ++i)
        {
            lint oldAlgoTimeSubSum = 0;
            lint new1AlgoTimeSubSum = 0;
            lint new2AlgoTimeSubSum = 0;
            lint new3AlgoTimeSubSum = 0;

            // Запускаем насколько раз программу на одном и том же наборе данных
            for (uint j = 0; j < TEST_COUNT; ++j)
            {
                stringstream mapName;
                mapName << "map_" << i << ".txt";

                cout << "Run test from " << setw(20) << mapFolders[k] + '/' + mapName.str() << " || Attempt " << setw(3) << j + 1 << " of " << setw(3) << TEST_COUNT << '\r';

                ofstream logFile;
                string logFileName = "log.txt";

                //logFile.open(testResFolder + logFileName, ios_base::app);
                //logFile << "<< Run old agent for map " << mapFolders[k] << "/" << mapName.str() << " >>" << endl;
                //oldAlgoTimeSubSum += runAgent(TEST_FOLDER + mapFolders[k] + "/" + mapName.str(), !aStarOn, logFile, !fullRun);
                //logFile.close();

                logFile.open(TEST_RESULTS_FOLDER + logFileName, ios_base::app);
                logFile << "<< Run new agent for map" << mapFolders[k] << "/" << mapName.str() << " >>" << endl;
                new1AlgoTimeSubSum += runAgent(TEST_FOLDER + mapFolders[k] + "/" + mapName.str(), aStarOn, logFile, !fullRun, *hDist);
                logFile.close();

                logFile.open(TEST_RESULTS_FOLDER + logFileName, ios_base::app);
                logFile << "<< Run new agent for map" << mapFolders[k] << "/" << mapName.str() << " >>" << endl;
                new2AlgoTimeSubSum += runAgent(TEST_FOLDER + mapFolders[k] + "/" + mapName.str(), aStarOn, logFile, !fullRun, *hDist2);
                logFile.close();

                logFile.open(TEST_RESULTS_FOLDER + logFileName, ios_base::app);
                logFile << "<< Run new agent for map" << mapFolders[k] << "/" << mapName.str() << " >>" << endl;
                new3AlgoTimeSubSum += runAgent(TEST_FOLDER + mapFolders[k] + "/" + mapName.str(), aStarOn, logFile, !fullRun, *hDist3);
                logFile.close();
            }

            cout << endl;

            oldAlgoTimeSum += oldAlgoTimeSubSum / TEST_COUNT;
            new1AlgoTimeSum += new1AlgoTimeSubSum / TEST_COUNT;
            new2AlgoTimeSum += new2AlgoTimeSubSum / TEST_COUNT;
            new3AlgoTimeSum += new3AlgoTimeSubSum / TEST_COUNT;

            //oldRes << setw(10) << '"' + mapFolders[k] + '"' << setw(10) << oldAlgoTimeSubSum / TEST_COUNT << endl;
            new1Res << setw(10) << '"' + mapFolders[k] + '"' << setw(10) << new1AlgoTimeSubSum / TEST_COUNT << endl;
            new2Res << setw(10) << '"' + mapFolders[k] + '"' << setw(10) << new2AlgoTimeSubSum / TEST_COUNT << endl;
            new3Res << setw(10) << '"' + mapFolders[k] + '"' << setw(10) << new3AlgoTimeSubSum / TEST_COUNT << endl;

            cmpRes << setw(10) << '"' + mapFolders[k] + '"' << setw(10) << new1AlgoTimeSubSum / TEST_COUNT << setw(10) << new2AlgoTimeSubSum / TEST_COUNT << setw(10) << new3AlgoTimeSubSum / TEST_COUNT << endl;
        }

        //oldRes << setw(10) << '"' + mapFolders[k] + '"' << setw(10) << oldAlgoTimeSum / mapCounts[k] << endl;
        new1Res << setw(10) << '"' + mapFolders[k] + '"' << setw(10) << new1AlgoTimeSum / mapCounts[k] << endl;
        new2Res << setw(10) << '"' + mapFolders[k] + '"' << setw(10) << new2AlgoTimeSum / mapCounts[k] << endl;
        new3Res << setw(10) << '"' + mapFolders[k] + '"' << setw(10) << new3AlgoTimeSum / mapCounts[k] << endl;

        cmpRes << setw(10) << '"' + mapFolders[k] + '"' << setw(10) << new1AlgoTimeSum / mapCounts[k] << setw(10) << new2AlgoTimeSum / mapCounts[k] << setw(10) << new3AlgoTimeSum / mapCounts[k] << endl;

        //oldRes.close();
        new1Res.close();
        new2Res.close();
        new3Res.close();
        cmpRes.close();
    }

    cout << "<< Testing compleete! >>";

    return 0;
}

lint runAgent(string filePath, bool isAstarUse, ostream& out, bool isFullRun, heuristicDist hFun)
{
    // Печать информации об обозначении узлов и карты
    mapInfo = readMap(filePath);
    out << setw(3) << static_cast<int>(Node::AGENT) << " - Agent\n"
        << setw(3) << static_cast<int>(Node::WUMPUS) << " - Wumpus\n"
        << setw(3) << static_cast<int>(Node::PIT) << " - Pit\n"
        << setw(3) << static_cast<int>(Node::GOLD) << " - Gold\n"
        << setw(3) << static_cast<int>(Node::STENCH) << " - Stench\n"
        << setw(3) << static_cast<int>(Node::BREEZE) << " - Breeze" << endl;
    printMap(mapInfo.cave, mapInfo.nColumn, mapInfo.nRow, out);

    // Инициализация векторов
    not_safe_cells = vector<bool>(mapInfo.n);
    unknown_cells = vector<bool>(mapInfo.n);
    B = vector<bdd>(mapInfo.n);
    P = vector<bdd>(mapInfo.n);
    S = vector<bdd>(mapInfo.n);
    W = vector<bdd>(mapInfo.n);
    G = vector<bdd>(mapInfo.n);

    L = vector<bdd>(mapInfo.n);
    V = vector<bdd>(mapInfo.n);
    OK = vector<bdd>(mapInfo.n);
    L_next = vector<bdd>(mapInfo.n);
    V_next = vector<bdd>(mapInfo.n);
    OK_next = vector<bdd>(mapInfo.n);

    //cells = stack<int>(mapInfo.n);

    checked_cells = vector<bool>(mapInfo.n);
    safe_cells = vector<bool>(mapInfo.n);
    checked_cells[0] = true;
    safe_cells[0] = true;
    for (int i = 1; i < mapInfo.n; ++i)
    {
        checked_cells[i] = false;
        safe_cells[i] = false;
    }

    // Рассчёт количества логических переменных
    int maxFval = mapInfo.nColumn * mapInfo.nRow + 2 * max(mapInfo.nColumn, mapInfo.nRow);
    uint NUMS_FOR_F_VAL = log_2(maxFval);
    uint NUMS_FOR_NODE_VAL = log_2(mapInfo.n);
    int N_VAR = DEPEND_ON_NODE_VAR_COUNT * mapInfo.n + INDEPEND_FROM_NODE_VAR_COUNT + 2 * NUMS_FOR_NODE_VAL + NUMS_FOR_DIR + NUMS_FOR_F_VAL + 1;

    // Инициализация Buddy
    int countvar = 0;               // Сквозной счетчик для bdd
    bdd_init(10000000, 10000000);   // Выделяем память для 1000000 строк таблицы и кэш размером 100000
    bdd_setvarnum(N_VAR);           // Задаем количество булевых переменных

    bdd task = bddtrue; //Решение. Изначально true. Здесь будет находиться база
    bdd movements = bddtrue;

    // Статика
    if (isFullRun)
    {
        // Выделение bool переменных
        // для Вампуса 0..N
        // для запаха  N..N*2
        // для ям      N*2..N*3
        // для ветра   N*3..N*4
        // Для золота  N*4..N*5

        // для Вампуса
        for (int i = 0; i < mapInfo.n; i++)
        {
            W[i] = bdd_ithvar(countvar++);
        }

        // для запаха
        for (int i = 0; i < mapInfo.n; i++)
        {
            S[i] = bdd_ithvar(countvar++);
        }

        //для ям
        for (int i = 0; i < mapInfo.n; i++)
        {
            P[i] = bdd_ithvar(countvar++);
        }

        //для ветра
        for (int i = 0; i < mapInfo.n; i++)
        {
            B[i] = bdd_ithvar(countvar++);
        }

        // Для золота
        for (int i = 0; i < mapInfo.n; i++)
        {
            G[i] = bdd_ithvar(countvar++);
        }

        //для Stench (запах)
        Stench = bdd_ithvar(countvar++);

        //для Breeze (ветер)
        Breeze = bdd_ithvar(countvar++);
        
        //для Scream (???)
        Scream = bdd_ithvar(countvar++);
    }

    // Динамика
    if (isFullRun)
    {
        // для нахождения агента в клетке
        for (int i = 0; i < mapInfo.n; i++)
        {
            L[i] = bdd_ithvar(countvar++);
        }

        // для посещение клетки агентом
        for (int i = 0; i < mapInfo.n; i++)
        {
            V[i] = bdd_ithvar(countvar++);
        }

        //для безопасной клетки
        for (int i = 0; i < mapInfo.n; i++)
        {
            OK[i] = bdd_ithvar(countvar++);
        }

        //для стрелы
        HaveArrow = bdd_ithvar(countvar++); //есть стрела в текущей клетке

        //для того что Вампус жив
        WumpusAlive = bdd_ithvar(countvar++);

        //для направлений
        East = bdd_ithvar(countvar++);
        South = bdd_ithvar(countvar++);
        West = bdd_ithvar(countvar++);
        North = bdd_ithvar(countvar++);

        //для действий (форвард, тёрнлефт, тёрнрайт)
        Forward = bdd_ithvar(countvar++);
        TurnLeft = bdd_ithvar(countvar++);
        TurnRight = bdd_ithvar(countvar++);
        Grab = bdd_ithvar(countvar++);
        Climb = bdd_ithvar(countvar++);
        Shoot = bdd_ithvar(countvar++);

        //для состояний агента (золото, вылез ли из пещеры?)
        ClimbedOut = bdd_ithvar(countvar++);
        HaveGold = bdd_ithvar(countvar++);
    }

    // ПЕРЕМЕННЫЕ ДЛЯ СЛЕДУЮЩЕГО СОСТОЯНИЯ
    TimeDependentActions actionsNext;
    if (isFullRun)
    {
        for (int i = 0; i < mapInfo.n; i++)
        {
            L_next[i] = bdd_ithvar(countvar++);
        }

        for (int i = 0; i < mapInfo.n; i++)
        {
            V_next[i] = bdd_ithvar(countvar++);
        }

        for (int i = 0; i < mapInfo.n; i++)
        {
            OK_next[i] = bdd_ithvar(countvar++);
        }

        HaveArrow_next = bdd_ithvar(countvar++); //нет стрелы

        //для того что Вампус жив
        WumpusAlive_next = bdd_ithvar(countvar++);

        //для направлений
        East_next = bdd_ithvar(countvar++);
        South_next = bdd_ithvar(countvar++);
        West_next = bdd_ithvar(countvar++);
        North_next = bdd_ithvar(countvar++);

        //для действий (форвард, тёрнлефт, тёрнрайт)
        actionsNext.Forward = bdd_ithvar(countvar++);
        actionsNext.TurnLeft = bdd_ithvar(countvar++);
        actionsNext.TurnRight = bdd_ithvar(countvar++);

        Grab_next = bdd_ithvar(countvar++);
        Climb_next = bdd_ithvar(countvar++);
        Shoot_next = bdd_ithvar(countvar++);

        actionsNext.Grab = Grab_next;
        actionsNext.Climb = Climb_next;
        actionsNext.Shoot = Shoot_next;

        //для состояний агента (золото, вылез ли из пещеры?)
        ClimbedOut_next = bdd_ithvar(countvar++);
        HaveGold_next = bdd_ithvar(countvar++);
    }

    // Переменные для функции оценки
    vector<bdd> fValues;
    int nodesCout = mapInfo.nColumn * mapInfo.nRow;
    for (int i = 0; i < maxFval; ++i)
    {
        fValues.push_back(bddtrue);
        for (int j = 0; j < NUMS_FOR_F_VAL; ++j)
        {
            if (((i >> j) & 1) == 1)
            {
                fValues[i] &= bdd_ithvar(countvar + j);
            }
            else
            {
                fValues[i] &= !bdd_ithvar(countvar + j);
            }
        }
    }
    countvar += NUMS_FOR_F_VAL;

    if (isFullRun)
    {
        // НАЧАЛЬНАЯ БЗ АГЕНТА                                          ПРОВЕРЕНО
        task &= L[0];
        task &= V[0];
        task &= OK[0];

        for (int i = 1; i < mapInfo.n; i++)
        {
            task &= !L[i];
            task &= !V[i];
        }

        task &= !P[0]; // в текущей клетке нет ямы
        task &= !W[0]; // в текущей клетке нет Вампуса

        task &= HaveArrow;    // есть стрела
        task &= WumpusAlive;  // Вампус жив

        task &= East;   // агент смотрит на восток
        task &= !West;  // агент не смотрит на запад
        task &= !South; // агент не смотрит на запад
        task &= !North; // агент не смотрит на запад

        task &= !Forward;   // агент не двигался вперёд
        task &= !TurnLeft;  // агент не поворачивал налево
        task &= !TurnRight; // агент не поворачивал направо

        task &= !Grab;  // агент не брал золото
        task &= !Shoot; // агент не стрелял
        task &= !Climb; // агент не совершал действие, чтобы выбирался из пещеры

        task &= !ClimbedOut; // агент не выбирался из пещеры
        task &= !HaveGold;   // у агента нет золота


        // Формируем базу знаний не зависящую от времени
        for (int i = 0; i < mapInfo.n; i++)
        {
            vector<int> neigbours = neighbourNodes(i, mapInfo.nRow, mapInfo.nColumn);

            // формируем tempB, tempP, tempS.
            // отдельно формируем tempW
            // tempW это формула вида: (x1*!x2*..*!xn) | (!x1*x2*!x3*..*!xn) | .. | (!x1*..!x(n-1)*xn)
            bdd tempP = bddfalse;
            bdd tempB = bddtrue;
            bdd tempW = bddfalse;
            bdd tempS = bddtrue;
            for (int i = 0; i < neigbours.size(); ++i)
            {
                tempP |= P[neigbours[i]];
                tempB &= B[neigbours[i]];
                tempS &= S[neigbours[i]];
                bdd temp = bddtrue;
                for (int j = 0; j < neigbours.size(); ++j)
                {
                    if (i == j)
                    {
                        temp &= W[neigbours[j]];
                    }
                    else
                    {
                        temp &= !W[neigbours[j]];
                    }
                }
                tempW |= temp;
            }

            task &= (!B[i] ^ tempP) &
                (!P[i] ^ tempB) &
                (!S[i] ^ tempW) &
                (!W[i] ^ tempS);
        }

        // Два правила про Вампуса
        {
            // Правило, что Вампус хотя бы один
            bdd temp_w = bddfalse;
            for (int i = 0; i < mapInfo.n; i++)
            {
                temp_w |= W[i];
            }
            task &= temp_w;

            // Правило, что на поле не более одного Вампуса
            for (int i = 0; i < mapInfo.n; i++)
            {
                for (int j = 0; j < mapInfo.n; j++)
                {
                    if (j != i)
                    {
                        task &= (W[i] >> !W[j]);
                    }
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


        // для изменения местоположения агента, пока не трогала (???)
        for (int i = 0; i < mapInfo.n; i++)
        {
            bdd temp = L[i] & !Forward;

            // если ячейка не в крайнем левом столбце
            if (i % mapInfo.nColumn != 0)
            {
                temp |= L[i - 1] & Forward & East;
            }
            // если ячейка не в крайнем правом столбце
            if ((i + 1) % mapInfo.nColumn != 0)
            {
                temp |= L[i + 1] & Forward & West;
            }
            // если ячейка не на самой нижней строке
            if (i < mapInfo.nColumn * (mapInfo.nRow - 1))
            {
                temp |= L[i + mapInfo.nColumn] & Forward & North;
            }
            // если ячейка не на самой верхней строке
            if (i >= mapInfo.nColumn)
            {
                temp |= L[i - mapInfo.nColumn] & Forward & South;
            }

            task &= !L_next[i] ^ temp;
        }

        // ???
        for (int i = 0; i < mapInfo.n; i++)
        {
            task &= !(V_next[i] ^ (V[i] | L[i]));
        }

        // переменные со стороны среды с учетом восприятия
        for (int i = 0; i < mapInfo.n; i++)
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

    }

    // СИМВОЛЬНЫЕ ВЫЧИСЛЕНИЯ

    vector<bdd> q(mapInfo.n);  // нештрихованные
    vector<bdd> qq(mapInfo.n); // штрихованные

    //const int xSize = 12; // TODO размер массива правилен?
    //bdd x[xSize];  // ???
    //// TODO это правильное выделение переменных?
    //for (int i = 0; i < xSize; i++)
    //{
    //    x[i] = bdd_ithvar(i);
    //}

    // Элементам массива q и qq присваиваются следующие значения
    // q[i]  = x[0] & .. & x[N_LOG - 1]
    // qq[i] = x[N_LOG] & .. & x[2 * N_LOG - 1]
    // Данные присвоения соответствуют следующему порядку
    // q[0] = 0..00 <=> !x[0] & .. & !x[N_LOG - 2] & !x[N_LOG - 1]
    // q[1] = 0..01 <=> !x[0] & .. & !x[N_LOG - 2] &  x[N_LOG - 1]
    // q[2] = 0..10 <=> !x[0] & .. &  x[N_LOG - 2] & !x[N_LOG - 1]
    // ...
    // q[N] = ... (последовательность из 0 и 1, которая в двоичном коде является представлением числа N)
    for (int i = 0; i < mapInfo.n; ++i)
    {
        q[i] = bddtrue;
        qq[i] = bddtrue;
        for (int j = 0; j < NUMS_FOR_NODE_VAL; ++j)
        {
            int jj = j + countvar;
            if (((i >> (NUMS_FOR_NODE_VAL - 1 - j)) & 1) == 1)
            {
                q[i] &= bdd_ithvar(jj);
                qq[i] &= bdd_ithvar(jj + NUMS_FOR_NODE_VAL);
            }
            else
            {
                q[i] &= !bdd_ithvar(jj);
                qq[i] &= !bdd_ithvar(jj + NUMS_FOR_NODE_VAL);
            }
        }
    }
    countvar += 2 * NUMS_FOR_NODE_VAL;

    // направления, куда смотрит агент, для символьных вычислений
    Directions dirs;
    dirs.n = !bdd_ithvar(countvar) & !bdd_ithvar(countvar + 1); // 00
    dirs.s = !bdd_ithvar(countvar) & bdd_ithvar(countvar + 1);  // 01
    dirs.e = bdd_ithvar(countvar) & !bdd_ithvar(countvar + 1);  // 10
    dirs.w = bdd_ithvar(countvar) & bdd_ithvar(countvar + 1);   // 11
    countvar += NUMS_FOR_DIR;

    // Описание переходов на графе для символьных вычислений
    bdd R = bddfalse;
    for (int i = 0; i < mapInfo.n; ++i)
    {
        // если ячейка не в крайнем левом столбце
        if (i % mapInfo.nColumn != 0)
        {
            R |= q[i] & dirs.w & qq[i - 1];
        }
        // если ячейка не в крайнем правом столбце
        if ((i + 1) % mapInfo.nColumn != 0)
        {
            R |= q[i] & dirs.e & qq[i + 1];
        }
        // если ячейка не на самой нижней строке
        if (i < mapInfo.nColumn * (mapInfo.nRow - 1))
        {
            R |= q[i] & dirs.s & qq[i + mapInfo.nColumn];
        }
        // если ячейка не на самой верхней строке
        if (i >= mapInfo.nColumn)
        {
            R |= q[i] & dirs.n & qq[i - mapInfo.nColumn];
        }
    }

    // начинаем всегда с 0 клетки
    int current_cell = 0;
    int cell_to_go = 0;

    vector<int> cells_visited_by_agent = { 0 };
    vector<bool> reverse_cells(mapInfo.n);
    for (int i = 0; i < mapInfo.n; ++i)
    {
        reverse_cells[i] = false;
    }

    bdd relation;
    vector <bdd> visited = { qq[current_cell] };
    vector <int> answer;
    bdd direction = dirs.e;
    stack <bdd> plan;
    stack <string> str_plan;
    stack <string> new_plan;
    stack <int> previous_cell;

    // Засекаем время старта алгоритма
    auto start = std::chrono::high_resolution_clock::now();

    if (isFullRun)
    {

        bool gold_flag = false;
        // Ходим по полю, пока не найдём золото или не будет ячеек, которые можно безопасно разведать
        while (gold_flag != true)
        {
            out << "Current cell is " << current_cell << endl;

            bdd percept = ask_and_send_percept(mapInfo.cave, current_cell);
            // Если мы на клетке с золотом
            if ((percept &= !G[current_cell]) == bddfalse)
            {
                out << "" << endl;
                out << "In cell " << current_cell << " is Gold! " << endl;
                out << "Now agent is going back to the exit" << endl;
                out << "" << endl;

                gold_flag = true;
            }
            // Если мы всё ещё в поисках золота
            else
            {
                task &= percept;

                cell_to_go = Enviroment(task, current_cell, out);

                bdd current_relation = R & q[current_cell];

                current_relation &= qq[cell_to_go];

                relation |= current_relation;

                find_path(plan, str_plan, current_relation, q[current_cell], q, qq, qq[cell_to_go], answer, visited, direction, actionsNext, dirs);
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
                        current_cell = move(current_cell, action, direction, dirs);
                        out << "action done by an agent - " << action << endl;
                        new_plan.pop();
                    }
                }
                cells_visited_by_agent.push_back(current_cell);
                out << "Current cell is " << current_cell << endl;
            }
        }
    }

    if (!isFullRun)
    {
        int goldCell = 0;
        for (int j = 0; j < mapInfo.n; ++j)
        {
            safe_cells[j] = true;
            for (int i = 0; i < mapInfo.cave[j].size(); ++i)
            {
                if (mapInfo.cave[j][i] == Node::GOLD)
                {
                    goldCell = j;
                }
                else if (mapInfo.cave[j][i] == Node::PIT || mapInfo.cave[j][i] == Node::WUMPUS)
                {
                    safe_cells[j] = false;
                }
            }
        }

        current_cell = goldCell;
    }

    // Поиск обратного пути, после того как нашли золото

    // Используем bddA*
    if (isAstarUse)
    {
        Plan plan;
        findPathAstar(plan, current_cell, R, 0, safe_cells, q, qq, mapInfo.nRow, mapInfo.nColumn, fValues, direction, dirs, actionsNext, hFun);

       for (int i = 0; i < plan.strPlan.size(); ++i)
        {
            string action = plan.strPlan[i];
            current_cell = move(current_cell, action, direction, dirs);
            out << "Action done by an agent - " << action << '\n';
            out << "Current cell is " << current_cell << endl;
        }
    }
    // Используем старую Сонину реализацию
    else
    {
        int startCell = current_cell;
        bool endFlag = false;
        bdd new_relation;
        // пока не вернулись назад
        while (current_cell != 0 && !endFlag)
        {
            int cellToGo = current_cell;
            previous_cell.push(current_cell);
            reverse_cells[current_cell] = true;
            new_relation = R & q[current_cell];


            vector<int> neighb = neighbourNodes(current_cell, mapInfo.nRow, mapInfo.nColumn);
            for (int i : neighbourNodes(current_cell, mapInfo.nRow, mapInfo.nColumn))
            {
                // если мы можем идти в соседнюю клетку
                if (safe_cells[i] == true && reverse_cells[i] == false)
                {
                    cellToGo = i;
                }
            }

            // если мы не можем найти путь к узлу назначения
            if (cellToGo == startCell)
            {
                endFlag = true;
            }
            // если нам некуда идти (мы в тупике), возвращаемся назад
            else if (cellToGo == current_cell)
            {

                // почему такая последовательность ??? (как это вообще работает?)
                current_cell = previous_cell.top();
                previous_cell.pop();
                current_cell = previous_cell.top();
                previous_cell.pop();
                out << "Current cell is " << current_cell << endl;
            }
            // Иначе меняем текущее положение на новое
            else
            {
                new_relation &= qq[cellToGo];

                find_path(plan, str_plan, R, q[current_cell], q, qq, qq[cellToGo], answer, visited, direction, actionsNext, dirs);
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
                        current_cell = move(current_cell, action, direction, dirs);
                        out << "action done by an agent - " << action << endl;
                        new_plan.pop();
                    }
                }

                current_cell = cellToGo;
                out << "Current cell is " << current_cell << endl;
            }
        }
    }

    // Время работы алгоритма
    auto diff = std::chrono::high_resolution_clock::now() - start; 
    auto microsec = std::chrono::duration_cast<std::chrono::microseconds>(diff);
    out << "" << endl;
    out << "Calculations took: " << microsec.count() << " microseconds" << endl;

    // Завершение
    bdd_done();
    return microsec.count();
}

int check_for_safety(bdd task, int current_cell, ostream& out)
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
        cell_to_go_next = cells.top();
        if ((task &= !P[current_cell]) == bddfalse)
            out << "In cell " << current_cell << " is Pit!" << endl;
        else if ((task &= !W[current_cell]) == bddfalse)
            out << "In cell " << current_cell << " is Wumpus!" << endl;
    }
    else //если неизвстна
    {
        unknown_cells[current_cell] = true; //говорим, что небезопасна
        //checked_cells[current_cell] = true; //добавляем в массив проверенных
        cell_to_go_next = cells.top();
    }
    return cell_to_go_next;
}

int find_unvisited(bdd task, int current_cell) 
{
    vector <int> neighbours_of_cell = neighbourNodes(current_cell, mapInfo.nRow, mapInfo.nColumn);

    for (int i = 0; i < neighbours_of_cell.size(); i++)
    {
        int check_if_safe = neighbours_of_cell[i];
        if (checked_cells[check_if_safe] == false)
        {
            cells.push(current_cell);
            current_cell = check_if_safe;
            break;
        }
    }
    return current_cell;
}

bdd ask_and_send_percept(vector<vector<Node>> Enviroment, int current_cell) 
{
    {
        bdd percept = bddtrue;
        for (int i = 0; i < Enviroment[current_cell].size(); i++)
        {
            switch (Enviroment[current_cell][i])
            {
            case Node::STENCH:
                percept &= S[current_cell];
                break;
            case Node::BREEZE:
                percept &= B[current_cell];
                break;
            case Node::GOLD:
                percept &= G[current_cell];
                break;
            default:
                percept &= !B[current_cell] & !S[current_cell] & !G[current_cell];
                break;
            }
        }
        return percept;
    }
}

int Enviroment(bdd task, int current_cell, ostream& out)
{
    int cell_to_check;
    int got_unsafe_cell;
    int count = 0;

    cell_to_check = find_unvisited(task, current_cell); //ближайшая непосещенная клетка, которую можно проверить на безопасность
    cell_to_check = check_for_safety(task, cell_to_check, out); //проверка на безопасность

    if (cell_to_check == current_cell)
    {
        vector<int> neighb = neighbourNodes(current_cell, mapInfo.nRow, mapInfo.nColumn);
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
                    return cell_to_check = check_for_safety(task, neighb[i], out);
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

int move(int cell, string action, bdd &direction, Directions &dir)
{
    if (direction == dir.n)
    {
        if (action == "forward")
            cell = cell - mapInfo.nRow;
        else if (action == "turnLeft")
            direction = dir.w;
        else if (action == "turnRight")
            direction = dir.e;
    }
    else if (direction == dir.s)
    {
        if (action == "forward")
            cell = cell + mapInfo.nRow;
        else if (action == "turnLeft")
            direction = dir.e;
        else if (action == "turnRight")
            direction = dir.w;
    }
    else if (direction == dir.e)
    {
        if (action == "forward")
            cell = cell + 1;
        else if (action == "turnLeft")
            direction = dir.n;
        else if (action == "turnRight")
            direction = dir.s;
    }
    else if (direction == dir.w)
    {
        if (action == "forward")
            cell = cell -1;
        else if (action == "turnLeft")
            direction = dir.s;
        else if (action == "turnRight")
            direction = dir.n;
    }
    return cell;
}

// Увы, я не могу понять, какова цель этой функции 
// (есть только стойкое ощущение, что функция просто ищет последовательность действий для перехода из одной клетки в соседнюю)
void find_path(stack <bdd>& plan, stack <string>& str_plan,
               bdd relation, bdd first_state, vector<bdd>& q, vector<bdd>& qq,
               bdd wished_state, vector<int> answer, vector<bdd> visited, bdd& direction,
               TimeDependentActions& actionsNext,
               Directions& directions)
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

            // если мы можем осуществить переход за один шаг в конечную вершину ???
            if ((X_R & qq[i]) != bddfalse && qq[i] == wished_state)
            {
                answer.push_back(i);
                dostijima = true;
                flag = true;
                visited.push_back(qq[i]);
                if (direction == directions.n)
                    dir = 1;
                else if (direction == directions.s)
                    dir = 2;
                else if (direction == directions.e)
                    dir = 3;
                else if (direction == directions.w)
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
                        if ((X_R & qq[i] & directions.e) != bddfalse)
                        {
                            str_plan.push("turnRight");
                            plan.push(TurnRight_next);
                            str_plan.push("forward");
                            plan.push(Forward_next);
                            //direction = e;
                        }
                        else if ((X_R & qq[i] & directions.w) != bddfalse)
                        {
                            str_plan.push("turnLeft");
                            plan.push(TurnLeft_next);
                            str_plan.push("forward");
                            plan.push(Forward_next);
                            //direction = w;
                        }
                        else if ((X_R & qq[i] & directions.s) != bddfalse)
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
                        if ((X_R & qq[i] & directions.e) != bddfalse)
                        {
                            str_plan.push("turnLeft");
                            plan.push(TurnLeft_next);
                            str_plan.push("forward");
                            plan.push(Forward_next);
                            //direction = e;
                        }
                        else if ((X_R & qq[i] & directions.w) != bddfalse)
                        {
                            str_plan.push("turnRight");
                            plan.push(TurnRight_next);
                            str_plan.push("forward");
                            plan.push(Forward_next);
                            //direction = w;
                        }
                        else if ((X_R & qq[i] & directions.n) != bddfalse)
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
                        if ((X_R & qq[i] & directions.s) != bddfalse)
                        {
                            str_plan.push("turnRight");
                            plan.push(TurnRight_next);
                            str_plan.push("forward");
                            plan.push(Forward_next);
                            //direction = s;
                        }
                        else if ((X_R & qq[i] & directions.n) != bddfalse)
                        {
                            str_plan.push("turnLeft");
                            plan.push(TurnLeft_next);
                            str_plan.push("forward");
                            plan.push(Forward_next);
                            //direction = n;
                        }
                        else if ((X_R & qq[i] & directions.w) != bddfalse)
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
                        if ((X_R & qq[i] & directions.s) != bddfalse)
                        {
                            str_plan.push("turnLeft");
                            plan.push(TurnLeft_next);
                            str_plan.push("forward");
                            plan.push(Forward_next);
                            //direction = s;
                        }
                        else if ((X_R & qq[i] & directions.n) != bddfalse)
                        {
                            str_plan.push("turnRight");
                            plan.push(TurnRight_next);
                            str_plan.push("forward");
                            plan.push(Forward_next);
                            //direction = n;
                        }
                        else if ((X_R & qq[i] & directions.e) != bddfalse)
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
                //  cout << answer[k];
                //  out << answer[k];
                //}
                //out << endl;
                //out.close();
                //cout << endl;
                //break;
            }
            // иначе если ???
            // у нас в X_R уже включена first_state !
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
                    if (direction == directions.n)
                        dir = 1;
                    else if (direction == directions.s)
                        dir = 2;
                    else if (direction == directions.e)
                        dir = 3;
                    else if (direction == directions.w)
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
                            if ((X_R & qq[i] & directions.e) != bddfalse)
                            {
                                str_plan.push("turnRight");
                                plan.push(TurnRight_next);
                                str_plan.push("forward");
                                plan.push(Forward_next);
                                //direction = e;
                            }
                            else if ((X_R & qq[i] & directions.w) != bddfalse)
                            {
                                str_plan.push("turnLeft");
                                plan.push(TurnLeft_next);
                                str_plan.push("forward");
                                plan.push(Forward_next);
                                //direction = w;
                            }
                            else if ((X_R & qq[i] & directions.s) != bddfalse)
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
                            if ((X_R & qq[i] & directions.e) != bddfalse)
                            {
                                str_plan.push("turnLeft");
                                plan.push(TurnLeft_next);
                                str_plan.push("forward");
                                plan.push(Forward_next);
                                //direction = e;
                            }
                            else if ((X_R & qq[i] & directions.w) != bddfalse)
                            {
                                str_plan.push("turnRight");
                                plan.push(TurnRight_next);
                                str_plan.push("forward");
                                plan.push(Forward_next);
                                //direction = w;
                            }
                            else if ((X_R & qq[i] & directions.n) != bddfalse)
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
                            if ((X_R & qq[i] & directions.s) != bddfalse)
                            {
                                str_plan.push("turnRight");
                                plan.push(TurnRight_next);
                                str_plan.push("forward");
                                plan.push(Forward_next);
                                //direction = s;
                            }
                            else if ((X_R & qq[i] & directions.n) != bddfalse)
                            {
                                str_plan.push("turnLeft");
                                plan.push(TurnLeft_next);
                                str_plan.push("forward");
                                plan.push(Forward_next);
                                //direction = n;
                            }
                            else if ((X_R & qq[i] & directions.w) != bddfalse)
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
                            if ((X_R & qq[i] & directions.s) != bddfalse)
                            {
                                str_plan.push("turnLeft");
                                plan.push(TurnLeft_next);
                                str_plan.push("forward");
                                plan.push(Forward_next);
                                //direction = s;
                            }
                            else if ((X_R & qq[i] & directions.n) != bddfalse)
                            {
                                str_plan.push("turnRight");
                                plan.push(TurnRight_next);
                                str_plan.push("forward");
                                plan.push(Forward_next);
                                //direction = n;
                            }
                            else if ((X_R & qq[i] & directions.e) != bddfalse)
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
                                find_path(plan, str_plan, relation, q[i], q, qq, wished_state, answer, visited, direction, actionsNext, directions);
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
        else
        {
            X_pr = first_state;
        }
    }
}

