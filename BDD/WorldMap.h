#pragma once
#include <vector>
#include <fstream>
#include <string>

using namespace std;

typedef unsigned int uint;

// TODO add to namespace?

constexpr uint N_COLUMN = 4;                // ������ ����
constexpr uint N_ROW =    4;                // ����� ����
constexpr uint N =        N_COLUMN * N_ROW; // ���-�� �����
constexpr uint N_LOG =    4;                // ���������� �������� ���������� ����������� ��� ����������� N �����

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

//����� 4�4
vector<std::vector<int>> real_cave2 = { {1},     {0},  {33}, {3},
                                        {0},     {33}, {3},  {33},
                                        {0},     {22}, {33}, {0},
                                        {4, 22}, {2},  {22}, {0}};

//vector<std::vector<int>> real_cave2 = { {Node::Agent},              {Node::None},   {Node::Breeze}, {Node::Pit},
//                                        {Node::None},               {Node::Breeze}, {Node::Pit},    {Node::Breeze},
//                                        {Node::None},               {Node::Stench}, {Node::Breeze}, {Node::None},
//                                        {Node::Gold, Node::Stench}, {Node::Wumpus}, {Node::Stench}, {Node::None} };

//����� 3�3
//vector<std::vector<Node>> real_cave3 = { {Node::Agent},  {Node::Breeze}, {Node::Pit},
//                                         {Node::Breeze}, {Node::None},   {Node::Breeze, Node::Gold},
//                                         {Node::Pit},    {Node::Breeze}, {Node::Pit, Node::Wumpus}};

// ����� �����
void printMap(vector<vector<Node>> map, uint nColumn = N_COLUMN, uint nRow = N_ROW, ostream &out=cout)
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

//vector<std::vector<int>> readFromFile(const string& path)
//{
//    ifstream in;
//    in.open();
//    for ()
//    {
//
//    }
//}