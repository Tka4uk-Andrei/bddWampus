#pragma once

#pragma comment(lib,"bdd.lib")
#pragma warning (disable : 26444)

#include <vector>
#include <stack>
#include <string>
#include <math.h>

#include "bdd.h"
#include "Types.h"

using namespace std;

typedef uint(*heuristicDist)(int, int, int);

/// <summary>
///     Название действия "движение вперёд"
/// </summary>
string FORWARD_STR    = "forward";

/// <summary>
///     Название действия "поворот налево"
/// </summary>
string TURN_LEFT_STR  = "turnLeft";

/// <summary>
///     Название действия "поворот направо"
/// </summary>
string TURN_RIGHT_STR = "turnRight";

/// <summary>
///     План действий для агента
/// </summary>
struct Plan
{
    /// <summary>
    ///     Описание последовательности дествий в формате BDD
    /// </summary>
    vector<bdd> bddPlan;

    /// <summary>
    ///     Описание последовательности действий в текстовом формате
    /// </summary>
    vector<string> strPlan;
};

/// <summary>
///     Определение соседей относительно клетки cell
/// </summary>
/// <param name="cell"> -- номер ячейки, относительно которой определяются соседи </param>
/// <param name="nRow"> -- длина поля </param>
/// <param name="nColumn"> -- ширина поля </param>
/// <returns> Набор номеров соседних клеток </returns>
vector<int> neighbourNodes(int cell, uint nRow, uint nColumn);

/// <summary>
///     Проверка того что две клетки соседи
/// </summary>
/// <param name="cell"> -- номер ячейки, относительно которой идёт проверка потенциального соседа </param>
/// <param name="probe"> -- номер ячейки, которую проверяют на соседство </param>
/// <param name="nRow"> -- длина поля </param>
/// <param name="nColumn"> -- ширина поля </param>
/// <returns> истина, если соседи, иначе ложь </returns>
bool isNeighbour(int cell, int probe, uint nRow, uint nColumn);

/// <summary>
///     Получениие плана действий для перехода между соседними клетками
/// </summary>
/// <param name="plan"> -- План, в который добавляются действия </param>
/// <param name="currentDir"> -- Текущее направление агента </param>
/// <param name="destDir"> -- Направление, куда надо смотреть для перехода </param>
/// <param name="directions"> -- Переменная, описывающая направления движения </param>
/// <param name="actions"> -- Переменная, описывающая действия  агента </param>
void getNeighbourPlan(Plan& plan, bdd& currentDir, bdd& destDir, Directions& directions, TimeDependentActions& actions);

/// <summary>
///     Получение номера направления
/// </summary>
/// <param name="dir"> -- Направление, для которого нужно получить номер </param>
/// <param name="directions"> -- Переменная, описывающая направления движения </param>
/// <returns> 0 - север, 1 - запад, 2 - юг, 3 - восток </returns>
int getDirId(bdd& dir, Directions& directions);

/// <summary>
///     Эвристическая функция оценки стоимости достижения узла
/// </summary>
/// <param name="cur"> -- Текущий номер узла </param>
/// <param name="dest"> -- Номер конечного узла </param>
/// <param name="nColumn"> -- ширина поля </param>
/// <returns> числовое значение  </returns>
uint hDist(int cur, int dest, int nColumn)
{
    int width = abs(cur % nColumn - dest % nColumn);
    int height = abs(cur / nColumn - dest / nColumn);

    return 2 * max(width, height);
}

uint hDist2(int cur, int dest, int nColumn)
{
    int width = abs(cur % nColumn - dest % nColumn);
    int height = abs(cur / nColumn - dest / nColumn);

    return width + height;
}

uint hDist3(int cur, int dest, int nColumn)
{
    return 0;
}

/// <summary>
///     Поиск пути между указанными узлами. Если пути нет, поведение не предсказуемо
/// </summary>
/// <param name="plan"> -- План действий для достижения конечного узла </param>
/// <param name="startNodeNum"> -- Номер узела из которого строится маршрут </param>
/// <param name="relation"> --  </param>
/// <param name="endNodeNum"> -- Номер узела назначения </param>
/// <param name="safeCells"> -- Массив, содержащий информацию о безопасных ячейках </param>
/// <param name="q"> -- Массив узлов закодированных в BDD для текущего момента времени </param>
/// <param name="qq"> -- Массив узлов закодированных в BDD для следующего момента времени </param>
/// <param name="nRow"> -- Длина поля </param>
/// <param name="nColumn"> -- Ширина поля </param>
/// <param name="fValues"> -- Массив с преподсчитанными значениями функции оценки </param>
/// <param name="iniDir"> -- начальное направление агента </param>
void findPathAstar(Plan& plan, int startNodeNum, bdd& relation, int endNodeNum, vector<bool>& safeCells, vector<bdd>& q, vector<bdd>& qq, uint nRow, uint nColumn, vector<bdd>& fValues, bdd& iniDir, Directions& directions, TimeDependentActions& actions, heuristicDist hFun);


void findPathAstar(Plan& plan, int startNodeNum, bdd& relation, int endNodeNum, vector<bool>& safeCells, vector<bdd>& q, vector<bdd>& qq, uint nRow, uint nColumn, vector<bdd>& fValues, bdd& iniDir, Directions& directions, TimeDependentActions& actions, heuristicDist hFun)
{
    uint nodeCount = nRow * nColumn;
    stack<pair<int, int>> keyNodes;
    vector<bool> visited(nodeCount);
    for (int i = 0; i < nodeCount; ++i)
    {
        visited[i] = false;
    }

    // формируем отношения переходов на основании информации о безопасных клетках
    bdd relations = bddfalse;
    for (int node = 0; node < nodeCount; ++node)
    {
        if (safeCells[node])
        {
            vector<int> neighbours = neighbourNodes(node, nRow, nColumn);
            for (int neighb : neighbours)
            {
                if (safeCells[neighb])
                {
                    relations |= q[node] & qq[neighb];
                }
            }
        }
    }

    // основная работа алгоритма поиска

    // инициализируем очередь
    bdd queue = bddfalse;
    queue |= fValues[hDist(startNodeNum, endNodeNum, nColumn)] & q[startNodeNum];
    while ((queue & q[endNodeNum]) == bddfalse && queue != bddfalse)
    {
        // извлекаем узел с минимальным значением
        // TODO сделать через обход по дереву
        // сначала находим число
        int fVal = 0;
        while ((queue & fValues[fVal]) == bddfalse)
        {
            ++fVal;
        }
        // затем находим номер узла 
        bdd nodesForOpening = queue & fValues[fVal];
        int node = 0;
        while ((nodesForOpening & q[node]) == bddfalse)
        {
            ++node;
        }

        // удаляем информацию об узле из очереди
        queue &= !(fValues[fVal] & q[node]);

        // находим соседей
        bdd neighbours = relations & q[node];

        // вычисляем значения функции оценки для каждого соседа и заносим величины в очередь
        auto neighbNodes = neighbourNodes(node, nRow, nColumn);
        for (int nextNode : neighbNodes)
        {
            if (!visited[nextNode])
            {
                if ((neighbours & qq[nextNode]) != bddfalse)
                {
                    int newFval = fVal - hFun(node, endNodeNum, nColumn) + 1 + hFun(nextNode, endNodeNum, nColumn);

                    // проверка на то, что соседний узел не был добавлен в очередь
                    if ((queue & q[nextNode]) != bddfalse)
                    {
                        bdd temp = queue & q[nextNode];
                        int fValNext = 0;
                        while ((temp & fValues[fValNext]) == bddfalse)
                        {
                            ++fValNext;
                        }

                        // удаляем старый элемент из очереди и добавляем новое значение
                        queue &= !(fValues[fValNext] & q[nextNode]);
                        queue |= (fValues[min(fValNext, newFval)] & q[nextNode]);
                    }
                    else
                    {
                        queue |= fValues[newFval] & q[nextNode];
                    }
                }
            }
        }

        // помечаем, что узел посещён
        visited[node] = true;

        // заносим в стек раскрытых вершин
        keyNodes.push(pair<int, int>(node, fVal - hFun(node, endNodeNum, nColumn)));
    }

    // Если пути до узла назначения не существует
    if (queue == bddfalse)
    {
        return;
    }

    // получаем итоговое значение
    bdd endInfo = queue & q[endNodeNum];
    int gVal = 0;
    while ((queue & fValues[gVal]) == bddfalse)
    {
        ++gVal;
    }

    // формирование порядка
    stack<int> nodePlan;
    nodePlan.push(endNodeNum);
    while (!keyNodes.empty())
    {
        // проверяем что узел входит в итоговый маршрут
        if (gVal - 1  == keyNodes.top().second && isNeighbour(nodePlan.top(), keyNodes.top().first, nRow, nColumn))
        {
            --gVal;
            nodePlan.push(keyNodes.top().first);
        }

        keyNodes.pop();
    }

    // Формирование плана
    // TODO move to previous cycle
    bdd curDir = iniDir;
    while (nodePlan.size() != 1)
    {
        int curNode = nodePlan.top();
        nodePlan.pop();

        bdd temp = relation & q[curNode] & qq[nodePlan.top()];

        if ((temp & directions.e) != bddfalse)
        {
            getNeighbourPlan(plan, curDir, directions.e, directions, actions);
            curDir = directions.e;
        }
        else if ((temp & directions.n) != bddfalse)
        {
            getNeighbourPlan(plan, curDir, directions.n, directions, actions);
            curDir = directions.n;
        }
        else if ((temp & directions.s) != bddfalse)
        {
            getNeighbourPlan(plan, curDir, directions.s, directions, actions);
            curDir = directions.s;
        }
        else
        {
            getNeighbourPlan(plan, curDir, directions.w, directions, actions);
            curDir = directions.w;
        }
    }
}

int getDirId(bdd& dir, Directions& directions)
{
    if (dir == directions.n)
    {
        return 0;
    }
    else if (dir == directions.w)
    {
        return 1;
    }
    else if (dir == directions.s)
    {
        return 2;
    }
    else
    {
        return 3;
    }
}

void getNeighbourPlan(Plan& plan, bdd& currentDir, bdd& destDir, Directions& directions, TimeDependentActions& actions)
{
    const int DIR_COUNT = 4;
    const int HALF_TURN_ROUND = DIR_COUNT / 2;

    int destDirId    = getDirId(destDir, directions);
    int currentDirId = getDirId(currentDir, directions);

    int leftTurnCount = (destDirId - currentDirId + DIR_COUNT) % DIR_COUNT;

    // Если повороты совершать не надо
    if (leftTurnCount == 0)
    {
        plan.bddPlan.push_back(actions.Forward);
        plan.strPlan.push_back(FORWARD_STR);
        return;
    }

    // Проверяем оптимально ли совершать левые повороты
    if (leftTurnCount <= HALF_TURN_ROUND)
    {
        for (int i = 0; i < leftTurnCount; ++i)
        {
            plan.bddPlan.push_back(actions.TurnLeft);
            plan.strPlan.push_back(TURN_LEFT_STR);
        }
    }
    // Иначе совершаем правые повороты
    else
    {
        for (int i = 0; i < DIR_COUNT - leftTurnCount; ++i)
        {
            plan.bddPlan.push_back(actions.TurnRight);
            plan.strPlan.push_back(TURN_RIGHT_STR);
        }
    }

    plan.bddPlan.push_back(actions.Forward);
    plan.strPlan.push_back(FORWARD_STR);
}

vector<int> neighbourNodes(int cell, uint nRow, uint nColumn)
{
    vector <int> neighbours;

    // если ячейка не в крайнем левом столбце
    if (cell % nColumn != 0)
    {
        neighbours.push_back(cell - 1);
    }
    // если ячейка не в крайнем правом столбце
    if ((cell + 1) % nColumn != 0)
    {
        neighbours.push_back(cell + 1);
    }
    // если ячейка не на самой нижней строке
    if (cell < nColumn * (nRow - 1))
    {
        neighbours.push_back(cell + nColumn);
    }
    // если ячейка не на самой верхней строке
    if (cell >= nColumn)
    {
        neighbours.push_back(cell - nColumn);
    }

    return neighbours;
}

bool isNeighbour(int cell, int probe, uint nRow, uint nColumn)
{
    // если ячейка не в крайнем левом столбце и она сосед слева
    if (cell % nColumn != 0 && cell - 1 == probe)
    {
        return true;
    }
    // если ячейка не в крайнем правом столбце и она сосед справа
    if ((cell + 1) % nColumn != 0 && cell + 1 == probe)
    {
        return true;
    }
    // если ячейка не на самой нижней строке и она сосед снизу
    if (cell < nColumn * (nRow - 1) && cell + nColumn == probe)
    {
        return true;
    }
    // если ячейка не на самой верхней строке и она сосед сверху
    if (cell >= nColumn && cell - nColumn == probe)
    {
        return true;
    }

    // если ячейка не соседняя
    return false;
}

