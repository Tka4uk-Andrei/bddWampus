#pragma once

#include <fstream>
#include <sstream>
#include <vector>
#include <string>

#include "Types.h"

using namespace std;

// TODO add to namespace?

// �������� ����� �������� ����
enum class Node : int
{
    NONE = 0,    // �����
    AGENT = 1,   // �����
    WUMPUS = 2,  // ������
    PIT = 3,     // ���
    GOLD = 4,    // ������
    STENCH = 22, // �����
    BREEZE = 33  // �������
};

// ��������� ����� ��������� (������: ���� 4x4)
// | 0  || 1  || 2  || 3  |
// | 4  || 5  || 6  || 7  |
// | 8  || 9  || 10 || 11 |
// | 12 || 13 || 14 || 15 |

// �.�. �� ����� ������ ����� ���� ��������� �������, �������� 
// ����� �� ���-�� �������� ����������, ����� �� ����� ��� ������ ���� �����

/// <summary>
///     ����� ���� �������
/// </summary>
struct Map 
{
    /// <summary>
    ///     ��������� ������
    /// </summary>
    vector <vector<Node>> cave;
    
    /// <summary>
    ///     ����� ����
    /// </summary>
    uint nRow;
    
    /// <summary>
    ///     ������ ����
    /// </summary>
    uint nColumn;
    
    /// <summary>
    ///     ����� ���������� ����� (nRow * nColumn)
    /// </summary>
    uint n;
};

/// <summary>
///     �������������� ����� � ���� Node
/// </summary>
/// <param name="val"> -- �����, ������� ������������� ������-�� �������� � Node </param>
/// <returns> ������������� �������� � ���� Node, ���� ����� ������ ������������� ������������ Node::NONE </returns>
Node intToNode(int val)
{
    switch (val)
    {
        case static_cast<int>(Node::NONE):
            return Node::NONE;

        case static_cast<int>(Node::AGENT):
            return Node::AGENT;

        case static_cast<int>(Node::WUMPUS):
            return Node::WUMPUS;

        case static_cast<int>(Node::PIT):
            return Node::PIT;

        case static_cast<int>(Node::GOLD):
            return Node::GOLD;

        case static_cast<int>(Node::STENCH):
            return Node::STENCH;

        case static_cast<int>(Node::BREEZE):
            return Node::BREEZE;

        default:
            return Node::NONE;
    }
}

/// <summary>
///     �������� ����� ���� ������� �� �����
/// </summary>
/// <param name="path"> -- ���� � �����, ����������� ������ </param>
/// <returns>
///     ���������� �����
/// </returns>
Map readMap(string path)
{
    Map map;

    ifstream in;
    in.open(path);

    in >> map.nColumn >> map.nRow;
    map.n = map.nColumn * map.nRow;
    string tt;
    getline(in, tt);
    for (int i = 0; i < map.n; ++i)
    {
        string str;
        getline(in, str);
        stringstream sStr;
        sStr << str;
        
        int val;
        vector<Node> nodeDescription;
        while (sStr >> val)
        {
            nodeDescription.push_back(intToNode(val));
        }
        map.cave.push_back(nodeDescription);
    }

    return map;
}

/// <summary>
///     ����� �����
/// </summary>
/// <param name="map"> -- �������� ����� ��� ������ </param>
/// <param name="out"> -- �����, ���� ���������� ���������� � ����� </param>
/// <param name="nColumn"> -- ������ ���� </param>
/// <param name="nRow"> -- ����� ���� </param>
void printMap(vector<vector<Node>> map, uint nColumn, uint nRow, ostream& out = cout)
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

