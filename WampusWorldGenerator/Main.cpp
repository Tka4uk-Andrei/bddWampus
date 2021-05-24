#include <iostream>
#include <fstream>
#include <sstream>
#include <ctime>
#include <iomanip>

#include "../BDD/WorldMap.h"

typedef unsigned int uint;

using namespace std;

/// <summary>
///     Проверка на наличие свойства в узле
/// </summary>
/// <param name="map"> -- карта </param>
/// <param name="cell"> -- номер ячейки для проверки </param>
/// <param name="attribute"> -- название признака для проверки </param>
/// <returns>
///     true  -- если признак уже есть; || false -- признак отсутствует
/// </returns>
bool isAttributeExist(vector<vector<Node>>& map, int cell, Node attribute)
{
    for (Node attr : map[cell])
    {
        if (attr == attribute)
        {
            return true;
        }
    }

    return false;
}


void addNodeToMap(vector<vector<Node>> &map, int cell, Node central, Node child, int nRows, int nColums)
{
    map[cell].push_back(central);

    vector <int> neighbours;

    if (child != Node::NONE)
    {
        // если ячейка не в крайнем левом столбце
        if (cell % nColums != 0)
        {
            neighbours.push_back(cell - 1);
        }
        // если ячейка не в крайнем правом столбце
        if ((cell + 1) % nColums != 0)
        {
            neighbours.push_back(cell + 1);
        }
        // если ячейка не на самой нижней строке
        if (cell < nColums * (nRows - 1))
        {
            neighbours.push_back(cell + nColums);
        }
        // если ячейка не на самой верхней строке
        if (cell >= nColums)
        {
            neighbours.push_back(cell - nColums);
        }

        for (int neighb : neighbours)
        {
            if (!isAttributeExist(map, neighb, child))
            {
                map[neighb].push_back(child);
            }
        }
    }
}

int main()
{
    bool programEnd = false;

    while (!programEnd)
    {
        int nRows;     // Длина поля
        int nColums;   // Ширина поля
        int n;         // Количество узлов
        int pitAmount; // Количество ям в поле
        int mapAmount; // Количество генерируемых карт

        cout << "Enter field size: colums and rows (or width and height)" << endl;
        cin >> nRows >> nColums;
        n = nRows * nColums;

        cout << "Enter amount of pits" << endl;
        cin >> pitAmount;

        cout << "Enter amount of maps" << endl;
        cin >> mapAmount;

        // TODO добавить проверки полей на возможность создания
        // Количество узлов не превышает RAND_MAX, количество ям меньше количества узлов на 3

        srand(time(0));
        for (int i = 0; i < mapAmount; ++i)
        {
            cout << "Generating map " << setw(4) << i + 1 << " of " << setw(4) << mapAmount << "\r";

            vector<int> occupiedNodes; // узлы которые могут быть заняты ямами и золотом и вампусом. Золото и вампус это последние два значения массива

            // формируем узлы, которые будут помечены как ямы, золото и вампус
            int generatedCount = 0; // количество сгенерированных функцией rand() занятых узлов
            while (generatedCount != pitAmount + 2) // добавляем 2 т.к. надо учесть клетку с золотом, вампусом
            {
                int newPitNode = 1 + rand() % (n - 1);

                // проверяем что узел уже не был добавлен
                bool isInList = false;
                for (int node : occupiedNodes)
                {
                    if (node == newPitNode)
                    {
                        isInList = true;
                    }
                }

                if (!isInList)
                {
                    occupiedNodes.push_back(newPitNode);
                    ++generatedCount;
                }
            }

            // Формируем карту
            vector<vector<Node>>  map(n);  // создаём пустую карту
            map[0].push_back(Node::AGENT);
            for (int j = 0; j < pitAmount; ++j)
            {
                addNodeToMap(map, occupiedNodes[j], Node::PIT, Node::BREEZE, nRows, nColums);
            }
            addNodeToMap(map, occupiedNodes[pitAmount], Node::GOLD, Node::NONE, nRows, nColums);
            addNodeToMap(map, occupiedNodes[pitAmount + 1], Node::WUMPUS, Node::STENCH, nRows, nColums);
            for (int j = 0; j < n; ++j)
            {
                if (map[j].size() == 0)
                {
                    map[j].push_back(Node::NONE);
                }
            }

            // печатаем результат в файл
            ofstream out;
            stringstream fileName;
            fileName << "map_" << i << ".txt";
            out.open(fileName.str());
            out << nColums << " " << nRows;
            for (int j = 0; j < n; ++j)
            {
                out << "\n";
                for (Node node : map[j])
                {
                    out << static_cast<int>(node) << " ";
                }
            }
            out << endl;
            out.close();
        }
        cout << endl;

        char c;
        cout << "Generate new set (y/n)? ";
        cin >> c;
        while (c != 'y' && c != 'n')
        {
            cout << "Wrong input type \'y\' (yes) or \'n\' (no) ";
            cin >> c;
        }

        if (c == 'y')
        {
            programEnd = true;
        }
    }
    cout << "Program finishes" << endl;

    return 0;
}

