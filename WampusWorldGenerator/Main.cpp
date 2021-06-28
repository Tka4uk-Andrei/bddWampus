#include <iostream>
#include <fstream>
#include <sstream>
#include <ctime>
#include <iomanip>

#include "../BDD/WorldMap.h"

typedef unsigned int uint;

using namespace std;

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
        // ���� ������ �� � ������� ����� �������
        if (cell % nColums != 0)
        {
            neighbours.push_back(cell - 1);
        }
        // ���� ������ �� � ������� ������ �������
        if ((cell + 1) % nColums != 0)
        {
            neighbours.push_back(cell + 1);
        }
        // ���� ������ �� �� ����� ������ ������
        if (cell < N_COLUMN * (nRows - 1))
        {
            neighbours.push_back(cell + nColums);
        }
        // ���� ������ �� �� ����� ������� ������
        if (cell >= N_COLUMN)
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
    int nRows;     // ����� ����
    int nColums;   // ������ ����
    int n;         // ���������� �����
    int pitAmount; // ���������� �� � ����
    int mapAmount; // ���������� ������������ ����

    cout << "Enter field size: colums and rows (or width and height)" << endl;
    cin >> nRows >> nColums;
    n = nRows * nColums;

    cout << "Enter amount of pits" << endl;
    cin >> pitAmount;

    cout << "Enter amount of maps" << endl;
    cin >> mapAmount;

    // TODO �������� �������� ����� �� ����������� ��������
    // ���������� ����� �� ��������� RAND_MAX, ���������� �� ������ ���������� ����� �� 3

    //srand(time(0));
    srand(5);
    for (int i = 0; i < mapAmount; ++i)
    {
        cout << "Generating map " << setw(4) << i + 1 << " of " << setw(4) << mapAmount << "\r";

        vector<int> occupiedNodes; // ���� ������� ����� ���� ������ ����� � ������� � ��������. ������ � ������ ��� ��������� ��� �������� �������

        // ��������� ����, ������� ����� �������� ��� ���, ������ � ������
        int generatedCount = 0; // ���������� ��������������� �������� rand() ������� �����
        while (generatedCount != pitAmount + 2) // ��������� 2 �.�. ���� ������ ������ � �������, ��������
        {
            int newPitNode = 1 + rand() % (n - 1);

            // ��������� ��� ���� ��� �� ��� ��������
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

        // ��������� �����
        vector<vector<Node>>  map(n);  // ������ ������ �����
        map[0].push_back(Node::AGENT);
        for (int j = 0; j < pitAmount; ++j) 
        {
            addNodeToMap(map, occupiedNodes[j], Node::PIT, Node::BREEZE, nRows, nColums);
        }
        addNodeToMap(map, occupiedNodes[pitAmount], Node::GOLD, Node::NONE, nRows, nColums);
        addNodeToMap(map, occupiedNodes[pitAmount + 1], Node::PIT, Node::BREEZE, nRows, nColums);
        for (int j = 0; j < n; ++j)
        {
            if (map[j].size() == 0)
            {
                map[j].push_back(Node::NONE);
            }
        }

        // �������� ��������� � ����
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

    return 0;
}

