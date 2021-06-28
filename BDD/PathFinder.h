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
///     �������� �������� "�������� �����"
/// </summary>
string FORWARD_STR    = "forward";

/// <summary>
///     �������� �������� "������� ������"
/// </summary>
string TURN_LEFT_STR  = "turnLeft";

/// <summary>
///     �������� �������� "������� �������"
/// </summary>
string TURN_RIGHT_STR = "turnRight";

/// <summary>
///     ���� �������� ��� ������
/// </summary>
struct Plan
{
    /// <summary>
    ///     �������� ������������������ ������� � ������� BDD
    /// </summary>
    vector<bdd> bddPlan;

    /// <summary>
    ///     �������� ������������������ �������� � ��������� �������
    /// </summary>
    vector<string> strPlan;
};

/// <summary>
///     ����������� ������� ������������ ������ cell
/// </summary>
/// <param name="cell"> -- ����� ������, ������������ ������� ������������ ������ </param>
/// <param name="nRow"> -- ����� ���� </param>
/// <param name="nColumn"> -- ������ ���� </param>
/// <returns> ����� ������� �������� ������ </returns>
vector<int> neighbourNodes(int cell, uint nRow, uint nColumn);

/// <summary>
///     �������� ���� ��� ��� ������ ������
/// </summary>
/// <param name="cell"> -- ����� ������, ������������ ������� ��� �������� �������������� ������ </param>
/// <param name="probe"> -- ����� ������, ������� ��������� �� ��������� </param>
/// <param name="nRow"> -- ����� ���� </param>
/// <param name="nColumn"> -- ������ ���� </param>
/// <returns> ������, ���� ������, ����� ���� </returns>
bool isNeighbour(int cell, int probe, uint nRow, uint nColumn);

/// <summary>
///     ���������� ����� �������� ��� �������� ����� ��������� ��������
/// </summary>
/// <param name="plan"> -- ����, � ������� ����������� �������� </param>
/// <param name="currentDir"> -- ������� ����������� ������ </param>
/// <param name="destDir"> -- �����������, ���� ���� �������� ��� �������� </param>
/// <param name="directions"> -- ����������, ����������� ����������� �������� </param>
/// <param name="actions"> -- ����������, ����������� ��������  ������ </param>
void getNeighbourPlan(Plan& plan, bdd& currentDir, bdd& destDir, Directions& directions, TimeDependentActions& actions);

/// <summary>
///     ��������� ������ �����������
/// </summary>
/// <param name="dir"> -- �����������, ��� �������� ����� �������� ����� </param>
/// <param name="directions"> -- ����������, ����������� ����������� �������� </param>
/// <returns> 0 - �����, 1 - �����, 2 - ��, 3 - ������ </returns>
int getDirId(bdd& dir, Directions& directions);

/// <summary>
///     ������������� ������� ������ ��������� ���������� ����
/// </summary>
/// <param name="cur"> -- ������� ����� ���� </param>
/// <param name="dest"> -- ����� ��������� ���� </param>
/// <param name="nColumn"> -- ������ ���� </param>
/// <returns> �������� ��������  </returns>
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
///     ����� ���� ����� ���������� ������. ���� ���� ���, ��������� �� ������������
/// </summary>
/// <param name="plan"> -- ���� �������� ��� ���������� ��������� ���� </param>
/// <param name="startNodeNum"> -- ����� ����� �� �������� �������� ������� </param>
/// <param name="relation"> --  </param>
/// <param name="endNodeNum"> -- ����� ����� ���������� </param>
/// <param name="safeCells"> -- ������, ���������� ���������� � ���������� ������� </param>
/// <param name="q"> -- ������ ����� �������������� � BDD ��� �������� ������� ������� </param>
/// <param name="qq"> -- ������ ����� �������������� � BDD ��� ���������� ������� ������� </param>
/// <param name="nRow"> -- ����� ���� </param>
/// <param name="nColumn"> -- ������ ���� </param>
/// <param name="fValues"> -- ������ � ���������������� ���������� ������� ������ </param>
/// <param name="iniDir"> -- ��������� ����������� ������ </param>
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

    // ��������� ��������� ��������� �� ��������� ���������� � ���������� �������
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

    // �������� ������ ��������� ������

    // �������������� �������
    bdd queue = bddfalse;
    queue |= fValues[hDist(startNodeNum, endNodeNum, nColumn)] & q[startNodeNum];
    while ((queue & q[endNodeNum]) == bddfalse && queue != bddfalse)
    {
        // ��������� ���� � ����������� ���������
        // TODO ������� ����� ����� �� ������
        // ������� ������� �����
        int fVal = 0;
        while ((queue & fValues[fVal]) == bddfalse)
        {
            ++fVal;
        }
        // ����� ������� ����� ���� 
        bdd nodesForOpening = queue & fValues[fVal];
        int node = 0;
        while ((nodesForOpening & q[node]) == bddfalse)
        {
            ++node;
        }

        // ������� ���������� �� ���� �� �������
        queue &= !(fValues[fVal] & q[node]);

        // ������� �������
        bdd neighbours = relations & q[node];

        // ��������� �������� ������� ������ ��� ������� ������ � ������� �������� � �������
        auto neighbNodes = neighbourNodes(node, nRow, nColumn);
        for (int nextNode : neighbNodes)
        {
            if (!visited[nextNode])
            {
                if ((neighbours & qq[nextNode]) != bddfalse)
                {
                    int newFval = fVal - hFun(node, endNodeNum, nColumn) + 1 + hFun(nextNode, endNodeNum, nColumn);

                    // �������� �� ��, ��� �������� ���� �� ��� �������� � �������
                    if ((queue & q[nextNode]) != bddfalse)
                    {
                        bdd temp = queue & q[nextNode];
                        int fValNext = 0;
                        while ((temp & fValues[fValNext]) == bddfalse)
                        {
                            ++fValNext;
                        }

                        // ������� ������ ������� �� ������� � ��������� ����� ��������
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

        // ��������, ��� ���� �������
        visited[node] = true;

        // ������� � ���� ��������� ������
        keyNodes.push(pair<int, int>(node, fVal - hFun(node, endNodeNum, nColumn)));
    }

    // ���� ���� �� ���� ���������� �� ����������
    if (queue == bddfalse)
    {
        return;
    }

    // �������� �������� ��������
    bdd endInfo = queue & q[endNodeNum];
    int gVal = 0;
    while ((queue & fValues[gVal]) == bddfalse)
    {
        ++gVal;
    }

    // ������������ �������
    stack<int> nodePlan;
    nodePlan.push(endNodeNum);
    while (!keyNodes.empty())
    {
        // ��������� ��� ���� ������ � �������� �������
        if (gVal - 1  == keyNodes.top().second && isNeighbour(nodePlan.top(), keyNodes.top().first, nRow, nColumn))
        {
            --gVal;
            nodePlan.push(keyNodes.top().first);
        }

        keyNodes.pop();
    }

    // ������������ �����
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

    // ���� �������� ��������� �� ����
    if (leftTurnCount == 0)
    {
        plan.bddPlan.push_back(actions.Forward);
        plan.strPlan.push_back(FORWARD_STR);
        return;
    }

    // ��������� ���������� �� ��������� ����� ��������
    if (leftTurnCount <= HALF_TURN_ROUND)
    {
        for (int i = 0; i < leftTurnCount; ++i)
        {
            plan.bddPlan.push_back(actions.TurnLeft);
            plan.strPlan.push_back(TURN_LEFT_STR);
        }
    }
    // ����� ��������� ������ ��������
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

    // ���� ������ �� � ������� ����� �������
    if (cell % nColumn != 0)
    {
        neighbours.push_back(cell - 1);
    }
    // ���� ������ �� � ������� ������ �������
    if ((cell + 1) % nColumn != 0)
    {
        neighbours.push_back(cell + 1);
    }
    // ���� ������ �� �� ����� ������ ������
    if (cell < nColumn * (nRow - 1))
    {
        neighbours.push_back(cell + nColumn);
    }
    // ���� ������ �� �� ����� ������� ������
    if (cell >= nColumn)
    {
        neighbours.push_back(cell - nColumn);
    }

    return neighbours;
}

bool isNeighbour(int cell, int probe, uint nRow, uint nColumn)
{
    // ���� ������ �� � ������� ����� ������� � ��� ����� �����
    if (cell % nColumn != 0 && cell - 1 == probe)
    {
        return true;
    }
    // ���� ������ �� � ������� ������ ������� � ��� ����� ������
    if ((cell + 1) % nColumn != 0 && cell + 1 == probe)
    {
        return true;
    }
    // ���� ������ �� �� ����� ������ ������ � ��� ����� �����
    if (cell < nColumn * (nRow - 1) && cell + nColumn == probe)
    {
        return true;
    }
    // ���� ������ �� �� ����� ������� ������ � ��� ����� ������
    if (cell >= nColumn && cell - nColumn == probe)
    {
        return true;
    }

    // ���� ������ �� ��������
    return false;
}

