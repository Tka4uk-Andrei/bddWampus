#pragma once

#pragma comment(lib,"bdd.lib")
#pragma warning (disable : 26444)

#include <vector>
#include <stack>
#include <string>
#include <math.h>

#include "bdd.h"
#include "Types.h"

// TODO complete docs

using namespace std;

string FORWARD_STR    = "forward";
string TURN_LEFT_STR  = "turnLeft";
string TURN_RIGHT_STR = "turnRight";

struct Plan
{
    vector<bdd> bddPlan;
    vector<string> strPlan;
};


/// <summary>
///     Определение соседей относительно клетки cell
/// </summary>
/// <param name="cell"> -- номер ячейки, относительно которой  </param>
/// <param name="nRow"> -- длина поля </param>
/// <param name="nColumn"> -- ширина поля </param>
/// <returns></returns>
vector<int> neighbourNodes(int cell, uint nRow, uint nColumn);


/// <summary>
///     Проверка того что две клетки соседи
/// </summary>
/// <param name="cell"></param>
/// <param name="probe"></param>
/// <param name="nRow"></param>
/// <param name="nColumn"></param>
/// <returns></returns>
bool isNeighbour(int cell, int probe, uint nRow, uint nColumn);


void getNeighbourPlan(Plan& plan, bdd& currentDir, bdd& destDir, Directions& directions, TimeDependentActions& actions);


int getDirId(bdd& dir, Directions& directions);

//void find_path(stack <bdd>& plan, stack <string>& str_plan, bdd relation, bdd first_state, bdd* q, bdd* qq, bdd wished_state, vector<int> answer, vector<bdd> visited, bdd& direction)
//{
//    bool flag = false;
//    bool dostijima = false;
//    int dir;
//    while (!flag && !dostijima)
//    {
//        bdd X_pr = first_state;
//        bdd X_R = first_state & relation & wished_state;
//
//        for (int i = 0; i < 16; i++)
//        {
//
//            if (to_stop_recursion == true)
//                break;
//            if ((X_R & qq[i]) != bddfalse && qq[i] == wished_state)
//            {
//                answer.push_back(i);
//                dostijima = true;
//                flag = true;
//                visited.push_back(qq[i]);
//                if (direction == n)
//                    dir = 1;
//                else if (direction == s)
//                    dir = 2;
//                else if (direction == e)
//                    dir = 3;
//                else if (direction == w)
//                    dir = 4;
//                switch (dir)
//                {
//                case 1: //север
//                {
//                    if ((X_R & qq[i] & direction) != bddfalse)
//                    {
//                        str_plan.push("forward");
//                        plan.push(Forward_next);
//                    }
//
//                    else if ((X_R & qq[i] & direction) == bddfalse)
//                    {
//                        if ((X_R & qq[i] & e) != bddfalse)
//                        {
//                            str_plan.push("turnRight");
//                            plan.push(TurnRight_next);
//                            str_plan.push("forward");
//                            plan.push(Forward_next);
//                            //direction = e;
//                        }
//                        else if ((X_R & qq[i] & w) != bddfalse)
//                        {
//                            str_plan.push("turnLeft");
//                            plan.push(TurnLeft_next);
//                            str_plan.push("forward");
//                            plan.push(Forward_next);
//                            //direction = w;
//                        }
//                        else if ((X_R & qq[i] & s) != bddfalse)
//                        {
//                            str_plan.push("turnLeft");
//                            plan.push(TurnLeft_next);
//                            str_plan.push("turnLeft");
//                            plan.push(TurnLeft_next);
//                            str_plan.push("forward");
//                            plan.push(Forward_next);
//                            //direction = s;
//                        }
//                    }
//                } break;
//                case 2: //юг
//                {
//                    if ((X_R & qq[i] & direction) != bddfalse)
//                    {
//                        str_plan.push("forward");
//                        plan.push(Forward_next);
//                    }
//                    else if ((X_R & qq[i] & direction) == bddfalse)
//                    {
//                        if ((X_R & qq[i] & e) != bddfalse)
//                        {
//                            str_plan.push("turnLeft");
//                            plan.push(TurnLeft_next);
//                            str_plan.push("forward");
//                            plan.push(Forward_next);
//                            //direction = e;
//                        }
//                        else if ((X_R & qq[i] & w) != bddfalse)
//                        {
//                            str_plan.push("turnRight");
//                            plan.push(TurnRight_next);
//                            str_plan.push("forward");
//                            plan.push(Forward_next);
//                            //direction = w;
//                        }
//                        else if ((X_R & qq[i] & n) != bddfalse)
//                        {
//                            str_plan.push("turnLeft");
//                            plan.push(TurnLeft_next);
//                            str_plan.push("turnLeft");
//                            plan.push(TurnLeft_next);
//                            str_plan.push("forward");
//                            plan.push(Forward_next);
//                            //direction = n;
//                        }
//                    }
//                } break;
//                case 3: //восток
//                {
//                    if ((X_R & qq[i] & direction) != bddfalse)
//                    {
//                        str_plan.push("forward");
//                        plan.push(Forward_next);
//                    }
//                    else if ((X_R & qq[i] & direction) == bddfalse)
//                    {
//                        if ((X_R & qq[i] & s) != bddfalse)
//                        {
//                            str_plan.push("turnRight");
//                            plan.push(TurnRight_next);
//                            str_plan.push("forward");
//                            plan.push(Forward_next);
//                            //direction = s;
//                        }
//                        else if ((X_R & qq[i] & n) != bddfalse)
//                        {
//                            str_plan.push("turnLeft");
//                            plan.push(TurnLeft_next);
//                            str_plan.push("forward");
//                            plan.push(Forward_next);
//                            //direction = n;
//                        }
//                        else if ((X_R & qq[i] & w) != bddfalse)
//                        {
//                            str_plan.push("turnLeft");
//                            plan.push(TurnLeft_next);
//                            str_plan.push("turnLeft");
//                            plan.push(TurnLeft_next);
//                            str_plan.push("forward");
//                            plan.push(Forward_next);
//                            //direction = w;
//                        }
//                    }
//                } break;
//                case 4: //запад
//                {
//                    if ((X_R & qq[i] & direction) != bddfalse)
//                    {
//                        str_plan.push("forward");
//                        plan.push(Forward_next);
//                    }
//                    else if ((X_R & qq[i] & direction) == bddfalse)
//                    {
//                        if ((X_R & qq[i] & s) != bddfalse)
//                        {
//                            str_plan.push("turnLeft");
//                            plan.push(TurnLeft_next);
//                            str_plan.push("forward");
//                            plan.push(Forward_next);
//                            //direction = s;
//                        }
//                        else if ((X_R & qq[i] & n) != bddfalse)
//                        {
//                            str_plan.push("turnRight");
//                            plan.push(TurnRight_next);
//                            str_plan.push("forward");
//                            plan.push(Forward_next);
//                            //direction = n;
//                        }
//                        else if ((X_R & qq[i] & e) != bddfalse)
//                        {
//                            str_plan.push("turnLeft");
//                            plan.push(TurnLeft_next);
//                            str_plan.push("turnLeft");
//                            plan.push(TurnLeft_next);
//                            str_plan.push("forward");
//                            plan.push(Forward_next);
//                            //direction = e;
//                        }
//                    }
//                }break;
//                }
//                //out.open("Route.txt", std::ios::app);
//                //for (int k = 0; k < answer.size(); k++)
//                //{
//                //	cout << answer[k];
//                //	out << answer[k];
//                //}
//                //out << endl;
//                //out.close();
//                //cout << endl;
//                //break;
//            }
//            else if ((X_R & qq[i] & first_state) != bddfalse)
//            {
//                int j = 0;
//                bool cflag = false;
//                while (j < visited.size() && !cflag)
//                {
//                    if (qq[i] == visited[j])
//                    {
//                        cflag = true;
//                    }
//                    j++;
//                }
//                if (!cflag)
//                {
//                    if (direction == n)
//                        dir = 1;
//                    else if (direction == s)
//                        dir = 2;
//                    else if (direction == e)
//                        dir = 3;
//                    else if (direction == w)
//                        dir = 4;
//                    switch (dir)
//                    {
//                    case 1: //север
//                    {
//                        if ((X_R & qq[i] & direction) != bddfalse)
//                        {
//                            str_plan.push("forward");
//                            plan.push(Forward_next);
//                        }
//
//                        else if ((X_R & qq[i] & direction) == bddfalse)
//                        {
//                            if ((X_R & qq[i] & e) != bddfalse)
//                            {
//                                str_plan.push("turnRight");
//                                plan.push(TurnRight_next);
//                                str_plan.push("forward");
//                                plan.push(Forward_next);
//                                //direction = e;
//                            }
//                            else if ((X_R & qq[i] & w) != bddfalse)
//                            {
//                                str_plan.push("turnLeft");
//                                plan.push(TurnLeft_next);
//                                str_plan.push("forward");
//                                plan.push(Forward_next);
//                                //direction = w;
//                            }
//                            else if ((X_R & qq[i] & s) != bddfalse)
//                            {
//                                str_plan.push("turnLeft");
//                                plan.push(TurnLeft_next);
//                                str_plan.push("turnLeft");
//                                plan.push(TurnLeft_next);
//                                str_plan.push("forward");
//                                plan.push(Forward_next);
//                                //direction = s;
//                            }
//                        }
//                    } break;
//                    case 2: //юг
//                    {
//                        if ((X_R & qq[i] & direction) != bddfalse)
//                        {
//                            str_plan.push("forward");
//                            plan.push(Forward_next);
//                        }
//                        else if ((X_R & qq[i] & direction) == bddfalse)
//                        {
//                            if ((X_R & qq[i] & e) != bddfalse)
//                            {
//                                str_plan.push("turnLeft");
//                                plan.push(TurnLeft_next);
//                                str_plan.push("forward");
//                                plan.push(Forward_next);
//                                //direction = e;
//                            }
//                            else if ((X_R & qq[i] & w) != bddfalse)
//                            {
//                                str_plan.push("turnRight");
//                                plan.push(TurnRight_next);
//                                str_plan.push("forward");
//                                plan.push(Forward_next);
//                                //direction = w;
//                            }
//                            else if ((X_R & qq[i] & n) != bddfalse)
//                            {
//                                str_plan.push("turnLeft");
//                                plan.push(TurnLeft_next);
//                                str_plan.push("turnLeft");
//                                plan.push(TurnLeft_next);
//                                str_plan.push("forward");
//                                plan.push(Forward_next);
//                                //direction = n;
//                            }
//                        }
//                    } break;
//                    case 3: //восток
//                    {
//                        if ((X_R & qq[i] & direction) != bddfalse)
//                        {
//                            str_plan.push("forward");
//                            plan.push(Forward_next);
//                        }
//                        else if ((X_R & qq[i] & direction) == bddfalse)
//                        {
//                            if ((X_R & qq[i] & s) != bddfalse)
//                            {
//                                str_plan.push("turnRight");
//                                plan.push(TurnRight_next);
//                                str_plan.push("forward");
//                                plan.push(Forward_next);
//                                //direction = s;
//                            }
//                            else if ((X_R & qq[i] & n) != bddfalse)
//                            {
//                                str_plan.push("turnLeft");
//                                plan.push(TurnLeft_next);
//                                str_plan.push("forward");
//                                plan.push(Forward_next);
//                                //direction = n;
//                            }
//                            else if ((X_R & qq[i] & w) != bddfalse)
//                            {
//                                str_plan.push("turnLeft");
//                                plan.push(TurnLeft_next);
//                                str_plan.push("turnLeft");
//                                plan.push(TurnLeft_next);
//                                str_plan.push("forward");
//                                plan.push(Forward_next);
//                                //direction = w;
//                            }
//                        }
//                    } break;
//                    case 4: //запад
//                    {
//                        if ((X_R & qq[i] & direction) != bddfalse)
//                        {
//                            str_plan.push("forward");
//                            plan.push(Forward_next);
//                        }
//                        else if ((X_R & qq[i] & direction) == bddfalse)
//                        {
//                            if ((X_R & qq[i] & s) != bddfalse)
//                            {
//                                str_plan.push("turnLeft");
//                                plan.push(TurnLeft_next);
//                                str_plan.push("forward");
//                                plan.push(Forward_next);
//                                //direction = s;
//                            }
//                            else if ((X_R & qq[i] & n) != bddfalse)
//                            {
//                                str_plan.push("turnRight");
//                                plan.push(TurnRight_next);
//                                str_plan.push("forward");
//                                plan.push(Forward_next);
//                                //direction = n;
//                            }
//                            else if ((X_R & qq[i] & e) != bddfalse)
//                            {
//                                str_plan.push("turnLeft");
//                                plan.push(TurnLeft_next);
//                                str_plan.push("turnLeft");
//                                plan.push(TurnLeft_next);
//                                str_plan.push("forward");
//                                plan.push(Forward_next);
//                                //direction = e;
//                            }
//                        }
//                    }break;
//                    }
//                    for (int j = 0; j < 9; j++)
//                    {
//                        if ((relation & q[i] & qq[j]) != bddfalse)
//                        {
//                            bool kflag = false;
//                            int k = 0;
//                            while (k < visited.size() && !kflag)
//                            {
//                                if (qq[j] == visited[k])
//                                {
//                                    kflag = true;
//                                }
//                                k++;
//                            }
//                            if (!kflag)
//                            {
//                                visited.push_back(qq[i]);
//                                answer.push_back(i);
//                                find_path(plan, str_plan, relation, q[i], q, qq, wished_state, answer, visited, direction);
//                                break;
//                            }
//                        }
//                    }
//                }
//            }
//        }
//        if (X_pr == first_state)
//        {
//            flag = true;
//            to_stop_recursion = true;
//        }
//        else X_pr = first_state;
//    }
//}
//

uint hDist(int cur, int dest, int nColumn)
{
    int width = abs(cur % nColumn - dest % nColumn);
    int height = abs(cur / nColumn - dest / nColumn);

    return 2 * max(width, height);
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
void findPathAstar(Plan& plan, int startNodeNum, bdd& relation, int endNodeNum, vector<bool>& safeCells, vector<bdd>& q, vector<bdd>& qq, uint nRow, uint nColumn, vector<bdd>& fValues, bdd& iniDir, Directions& directions, TimeDependentActions& actions)
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
                    int newFval = fVal - hDist(node, endNodeNum, nColumn) + 1 + hDist(nextNode, endNodeNum, nColumn);

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
        keyNodes.push(pair<int, int>(node, fVal - hDist(node, endNodeNum, nColumn)));
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

