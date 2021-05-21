#pragma once
#include <vector>
#include <fstream>
#include <string>

using namespace std;

typedef unsigned int uint;

// TODO add to namespace?

constexpr uint N_COLUMN = 4;                // Ширина поля
constexpr uint N_ROW =    4;                // Длина поля
constexpr uint N =        N_COLUMN * N_ROW; // Кол-во узлов
constexpr uint N_LOG =    4;                // Количество двоичных переменных необходимых для кодирования N узлов

// Описание ячеек игрового поля
enum class Node : int
{
    NONE = 0,    // Пусто
    AGENT = 1,   // Агент
    WUMPUS = 2,  // Вампус
    PIT = 3,     // Яма
    GOLD = 4,    // Золото
    STENCH = 22, // Запах
    BREEZE = 33  // Ветерок
};

// Нумерация ячеек следующая (пример: поле 4x4)
// | 0  || 1  || 2  || 3  |
// | 4  || 5  || 6  || 7  |
// | 8  || 9  || 10 || 11 |
// | 12 || 13 || 14 || 15 |

// Т.к. на одной клетке могут быть несколько свойств, возможно 
// лучше их как-то отдельно кодировать, чтобы на карте был всегда один номер

struct Map 
{
    vector <vector<Node>> cave;
    uint nRow;
    uint nColumn;
};

////карта 4х4
//vector<std::vector<int>> real_cave2 = { {1},     {0},  {33}, {3},
//                                        {0},     {33}, {3},  {33},
//                                        {0},     {22}, {33}, {0},
//                                        {4, 22}, {2},  {22}, {0}};

// карта 4х4
vector<std::vector<Node>> real_cave2 = {{Node::AGENT},              {Node::NONE},   {Node::BREEZE}, {Node::PIT},
                                        {Node::NONE},               {Node::BREEZE}, {Node::PIT},    {Node::BREEZE},
                                        {Node::NONE},               {Node::STENCH}, {Node::BREEZE}, {Node::NONE},
                                        {Node::GOLD, Node::STENCH}, {Node::WUMPUS}, {Node::STENCH}, {Node::NONE} };

//карта 3х3
//vector<std::vector<Node>> real_cave3 = { {Node::Agent},  {Node::Breeze}, {Node::Pit},
//                                         {Node::Breeze}, {Node::None},   {Node::Breeze, Node::Gold},
//                                         {Node::Pit},    {Node::Breeze}, {Node::Pit, Node::Wumpus}};

// Загрузка карты
Map readMap()
{
    Map map;

    return map;
}

// Вывод карты
void printMap(vector<vector<Node>> map, ostream& out = cout, uint nColumn = N_COLUMN, uint nRow = N_ROW)
{
    bool gold_flag = false;
    int k1 = 0;
    for (uint i = 0; i < nRow; i++)
    {
        for (uint j = 0; j < nColumn; ++j)
        {
            out << "( ";
            for (uint k = 0; k < map[i * nRow + j].size(); ++k)
            {
                out << static_cast<int>(map[i * nRow + j][k]) << " ";
            }
            out << ")";
        }
        out << endl;
    }
}
