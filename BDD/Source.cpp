#pragma comment(lib,"bdd.lib")
#pragma warning (disable : 26444)

#include"bdd.h"
#include<fstream>
#include <iostream>
#include <vector>
#include <cmath>
#include <time.h>
#include <map>
#include <stack>
#include <chrono>
#include <iomanip>

#include "WorldMap.h"

using namespace std;

ofstream out;

//N_DEPEND_ON_CELL*N_COLUMN*N_ROW+N_NOT_DEPEND_ON_CELL+18
#define N_DEPEND_ON_CELL 9
#define N_NOT_DEPEND_ON_CELL 18

#define N_FUNCTIONS_AT_CELL (3 + 14*4)
#define N_FUNCTIONS_IN_FIELD (4 + 3*4) // �� percept ����� ���� 4+3*2
#define N_VAR 207

vector<int> neighbourNodes(int cell); // ����������� ������� ������������ ������ cell
int find_unvisited(bdd task, int current_cell); //��������� �� ������� ������, ������ ��, ������� ��� �� ���������, ��������� �� �� ��������
bdd ask_and_send_percept(vector<vector <int>> Enviroment, int current_cell); // �� i-�� ������ ������� ���������� (percept)
int check_for_safety(bdd task, int current_cell); //��������� ������ �� ������������
int Enviroment(bdd task, int current_cell); //�������� ��� �������: ����� ������� � �������� �� ������������
void find_path(bdd relation, bdd first_state, bdd* q, bdd* qq, bdd wished_state, vector<int> answer, vector<bdd> visited, bdd& direction);
int move(int cell, string action, bdd& direction);

//���������� ���������� �����
bdd Stench; // �������� ������� 
bdd Breeze; // �������� ������� �������
bdd Scream; // ???

// ���������� ��������� �����, ���������, �� ������� �� �������
bdd B[N]; //�����
bdd P[N]; //���
bdd S[N]; //�����
bdd W[N]; //������
bdd G[N]; //������

// �����������, ���� ������� �����, ��� ���������� ����������
bdd n; // �����
bdd e; // �����
bdd s; // ��
bdd w; // ������

//���������� ��������� �����, ��������� �� �������
#pragma region Time-dependent environment state variables

bdd HaveArrow;//����� ������
bdd HaveArrow_next; //����� ������ � ��������� ���������

bdd WumpusAlive; //������ ���
bdd WumpusAlive_next;

bdd HaveGold; //����� ������
bdd HaveGold_next;

bdd ClimbedOut; //������� �� ������
bdd ClimbedOut_next;

#pragma endregion

//���������� ��������� ������, ��������� �� �������
#pragma region Time-dependent agent position variables

bdd L[N];      // ������� ��������� ������
bdd L_next[N]; // ��������� ������ � ��������� ������ �������

bdd North;      // ����� ������� �� �����
bdd North_next;

bdd West;       // ����� ������� �� �����
bdd West_next;

bdd South;      // ����� ������� �� ��
bdd South_next;

bdd East;      // ����� ������� �� ������
bdd East_next;

bdd V[N];       // ���������� ������� ������
bdd V_next[N];

bdd OK[N];      // ���������� ������
bdd OK_next[N];

#pragma endregion 

//���������� ��������, ��������� �� �������
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

vector<bool> checked_cells;  // ������ ����������� ������
//vector<bool> not_safe_cells; // ������ ������������ ������
//vector<bool> unknown_cells;  // ���� �� ����� ��������, ���������� ��� ���
vector<bool> safe_cells;     // ������ ���������� ������

bool not_safe_cells[N]; //������ ������������ ������ (�� �����???)
bool unknown_cells[N]; //���� �� ����� ��������, ���������� ��� ��� (�� �����???)

stack <int> cells[N]; //���� ��� �������� ���������� ������

// ����� ��� ���������� ����������
bool flag = false;
bool to_stop_recursion;

stack <bdd> plan;
stack <string> str_plan;

int main()
{
    // ���������. ������� �������������� �������, ����� ��������

    // ������������� Buddy
    int countvar = 0;             // �������� ������� ��� bdd
    bdd_init(10000000, 10000000); // �������� ������ ��� 1000000 ����� ������� � ��� �������� 100000
    bdd_setvarnum(N_VAR);         // ������ ���������� ������� ����������

    // ������������� ��������
    checked_cells[0] = true;
    safe_cells[0] = true;
    for (int i = 1; i < N; ++i)
    {
        checked_cells[i] = false;
        safe_cells[i] = false;
    }

    cout << setw(3) << static_cast<int>(Node::AGENT)  << " - Agent\n"
         << setw(3) << static_cast<int>(Node::WUMPUS) << " - Wumpus\n"
         << setw(3) << static_cast<int>(Node::PIT)    << " - Pit\n"
         << setw(3) << static_cast<int>(Node::GOLD)   << " - Gold\n"
         << setw(3) << static_cast<int>(Node::STENCH) << " - Stench\n"
         << setw(3) << static_cast<int>(Node::BREEZE) << " - Breeze" << endl;

    bdd task = bddtrue; //�������. ���������� true. ����� ����� ���������� ����
    bdd movements = bddtrue;

    // �������
    {
        // ��������� bool ����������
        // ��� ������� 0..N
        // ��� ������  N..N*2
        // ��� ��      N*2..N*3
        // ��� �����   N*3..N*4
        // ��� ������  N*4..N*5

        // TODO ����� ���������� � ���� ����

        // ��� �������
        for (int i = 0; i < N; i++)
        {
            W[i] = bdd_ithvar(countvar);
            countvar++;
        }

        // ��� ������
        for (int i = 0; i < N; i++)
        {
            S[i] = bdd_ithvar(countvar);
            countvar++;
        }

        //��� ��
        for (int i = 0; i < N; i++)
        {
            P[i] = bdd_ithvar(countvar);
            countvar++;
        }

        //��� �����
        for (int i = 0; i < N; i++)
        {
            B[i] = bdd_ithvar(countvar);
            countvar++;
        }

        // ��� ������
        for (int i = 0; i < N; i++)
        {
            G[i] = bdd_ithvar(countvar);
            countvar++;
        }

        //��� Stench (�����)
        Stench = bdd_ithvar(countvar);
        countvar++;

        //��� Breeze (�����)
        Breeze = bdd_ithvar(countvar);
        countvar++;

        //��� Scream
        Scream = bdd_ithvar(countvar);
        countvar++;
    }

    // ��������
    {
        // ��� ���������� ������ � ������
        for (int i = 0; i < N; i++)
        {
            L[i] = bdd_ithvar(countvar);
            countvar++;
        }

        // ��� ��������� ������ �������
        for (int i = 0; i < N; i++)
        {
            V[i] = bdd_ithvar(countvar);
            countvar++;
        }

        //��� ���������� ������
        for (int i = 0; i < N; i++)
        {
            OK[i] = bdd_ithvar(countvar);
            countvar++;
        }

        //��� ������
        HaveArrow = bdd_ithvar(countvar); //���� ������ � ������� ������
        countvar++;

        //��� ���� ��� ������ ���
        WumpusAlive = bdd_ithvar(countvar);
        countvar++;

        //��� �����������
        East = bdd_ithvar(countvar);
        countvar++;

        South = bdd_ithvar(countvar);
        countvar++;

        West = bdd_ithvar(countvar);
        countvar++;

        North = bdd_ithvar(countvar);
        countvar++;

        //��� �������� (�������, �������, �������)
        Forward = bdd_ithvar(countvar);
        countvar++;

        TurnLeft = bdd_ithvar(countvar);
        countvar++;

        TurnRight = bdd_ithvar(countvar);
        countvar++;

        Grab = bdd_ithvar(countvar);
        countvar++;

        Climb = bdd_ithvar(countvar);
        countvar++;

        Shoot = bdd_ithvar(countvar);
        countvar++;

        //��� ��������� ������ (������, ����� �� �� ������?)

        ClimbedOut = bdd_ithvar(countvar);
        countvar++;

        HaveGold = bdd_ithvar(countvar);
        countvar++;
    }

    // ���������� ��� ���������� ���������
    {
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

        HaveArrow_next = bdd_ithvar(countvar); //��� ������
        countvar++;

        //��� ���� ��� ������ ���
        WumpusAlive_next = bdd_ithvar(countvar);
        countvar++;

        //��� �����������
        East_next = bdd_ithvar(countvar);
        countvar++;

        South_next = bdd_ithvar(countvar);
        countvar++;

        West_next = bdd_ithvar(countvar);
        countvar++;

        North_next = bdd_ithvar(countvar);
        countvar++;

        //��� �������� (�������, �������, �������)
        Forward_next = bdd_ithvar(countvar);
        countvar++;

        TurnLeft_next = bdd_ithvar(countvar);
        countvar++;

        TurnRight_next = bdd_ithvar(countvar);
        countvar++;

        Grab_next = bdd_ithvar(countvar);
        countvar++;

        Climb_next = bdd_ithvar(countvar);
        countvar++;

        Shoot_next = bdd_ithvar(countvar);
        countvar++;

        //��� ��������� ������ (������, ����� �� �� ������?)

        ClimbedOut_next = bdd_ithvar(countvar);
        countvar++;

        HaveGold_next = bdd_ithvar(countvar);
        countvar++;
    }

    // ��������� �� ������                                          ���������
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

    task &= HaveArrow; //���� ������
    task &= WumpusAlive;

    task &= East;
    task &= !West;
    task &= !South;
    task &= !North;

    task &= !Forward;
    task &= !TurnLeft;
    task &= !TurnRight;

    task &= !Grab;
    task &= !Shoot;
    task &= !Climb;

    task &= !ClimbedOut;
    task &= !HaveGold;

    // ��������� ���� ������ �� ��������� �� �������
    for (int i = 0; i < N; i++)
    {
        vector<int> neigbours = neighbourNodes(i);

        // ��������� tempB, tempP, tempS.
        // �������� ��������� tempW
        // tempW ��� ������� ����: (x1*!x2*..*!xn) | (!x1*x2*!x3*..*!xn) | .. | (!x1*..!x(n-1)*xn)
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

    // ��� ������� ��� �������
    {
        // �������, ��� ������ ���� �� ����
        bdd temp_w = bddfalse;
        for (int i = 0; i < N; i++)
        {
            temp_w |= W[i];
        }
        task &= temp_w;

        // �������, ��� �� ���� �� ����� ������ �������
        for (int i = 0; i < N; i++)
        {
            for (int j = 0; j < N; j++)
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

    //����������� ������
    task &= (!North_next ^ ((TurnRight & West) | (TurnLeft & East) | (North & !TurnLeft & !TurnRight)));
    task &= (!South_next ^ ((TurnRight & East) | (TurnLeft & West) | (South & !TurnLeft & !TurnRight)));
    task &= (!East_next ^ ((TurnRight & North) | (TurnLeft & South) | (East & !TurnLeft & !TurnRight)));
    task &= (!West_next ^ ((TurnRight & South) | (TurnLeft & North) | (West & !TurnLeft & !TurnRight)));

    task &= (!HaveGold_next ^ (Grab & HaveGold));
    task &= (!ClimbedOut_next ^ (Climb & ClimbedOut));

    // ��� ��������� �������������� ������, ���� �� ������� (???)
    for (int i = 0; i < N; i++)
    {
        bdd temp = L[i] & !Forward;

        // ���� ������ �� � ������� ����� �������
        if (i % N_COLUMN != 0)
        {
            temp |= L[i - 1] & Forward & East;
        }
        // ���� ������ �� � ������� ������ �������
        if ((i + 1) % N_COLUMN != 0)
        {
            temp |= L[i + 1] & Forward & West;
        }
        // ���� ������ �� �� ����� ������ ������
        if (i < N_COLUMN * (N_ROW - 1))
        {
            temp |= L[i + N_COLUMN] & Forward & North;
        }
        // ���� ������ �� �� ����� ������� ������
        if (i >= N_COLUMN)
        {
            temp |= L[i - N_COLUMN] & Forward & South;
        }

        task &= !L_next[i] ^ temp;
    }

    // ???
    for (int i = 0; i < N; i++)
    {
        task &= !(V_next[i] ^ (V[i] | L[i]));
    }

    // ���������� �� ������� ����� � ������ ����������
    for (int i = 0; i < N; i++)
    {
        task &= (L[i] >> (!Breeze ^ B[i]));
        task &= (L[i] >> (!Stench ^ S[i]));
        task &= L[i] >> V[i];
        task &= ((!OK[i] | !P[i]) & (OK[i] | P[i] | W[i]) & (OK[i] | P[i] | WumpusAlive) & (!OK[i] | !W[i] | !WumpusAlive));
    }

    // �������������� �������� � ����������

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

    // ���������� ����������

    const int xSize = 12;
    bdd x[xSize];  // ???
    bdd q[16];  // ��������������
    bdd qq[16]; // ������������
    bdd qq1[16]; // ������������

    // ???
    for (int i = 0; i < xSize; i++)
    {
        x[i] = bdd_ithvar(i);
    }

    for (int i = 0; i < N; ++i)
    {
        q[i] = bddtrue;
        qq1[i] = bddtrue;
        for (int j = 0; j < N_LOG; ++j)
        {
            if (((i >> (N_LOG - 1 - j)) & 1) == 1) // don't ask me why
            {
                q[i] &= x[j];
                qq1[i] &= x[j + N_LOG];
            }
            else
            {
                q[i] &= !x[j];
                qq1[i] &= !x[j + N_LOG];
            }
        }
    }

    n = !x[8] & !x[9]; // 00
    s = !x[8] & x[9];  // 01
    e = x[8] & !x[9];  // 10
    w = x[8] & x[9];   // 11

    // �������� ��������� �� ����� ��� ���������� ����������
    bdd R = bddfalse;
    for (int i = 0; i < N; ++i)
    {
        // ���� ������ �� � ������� ����� �������
        if (i % N_COLUMN != 0)
        {
            R |= q[i]     | e | q[i - 1];
            R |= q[i - 1] | w | q[i];
        }
        // ���� ������ �� � ������� ������ �������
        if ((i + 1) % N_COLUMN != 0)
        {
            R |= q[i]     | w | q[i + 1];
            R |= q[i + 1] | e | q[i];
        }
        // ���� ������ �� �� ����� ������ ������
        if (i < N_COLUMN * (N_ROW - 1))
        {
            R |= q[i]            | s | q[i + N_COLUMN];
            R |= q[i + N_COLUMN] | n | q[i];
        }
        // ���� ������ �� �� ����� ������� ������
        if (i >= N_COLUMN)
        {
            R |= q[i]            | n | q[i - N_COLUMN];
            R |= q[i - N_COLUMN] | s | q[i];
        }
    }

    // �������� ������ � 0 ������
    int current_cell = 0;
    int cell_to_go = 0;

    vector<int> cells_visited_by_agent = { 0 };
    vector<bool> reverse_cells;
    for (int i = 0; i < N; ++i)
    {
        reverse_cells[i] = false;
    }

    // ������ �����
	int k1 = 0;
	for (unsigned p = 0; p < real_cave2.size(); p++)
	{
		cout << "( ";
		for (unsigned q = 0; q < real_cave2[p].size(); q++)
		{
			cout << real_cave2[p][q];
			cout << " ";
		}
		cout << ")";
		k1++;
		if (k1 % 4 == 0)
			cout << "\n";
	}

	bdd relation;
	vector <bdd> visited = { qq[current_cell] };
	vector <int> answer;
	bdd direction = e;
	stack <string> new_plan;
	stack <int> previous_cell;

    // �������� ����� ������ ���������
    auto start = std::chrono::high_resolution_clock::now();

    cout << "Current cell is 0" << endl;
    bool gold_flag = false;
    do { 

		bdd percept = ask_and_send_percept(real_cave2, current_cell);
		if ((percept &= !G[current_cell]) == bddfalse)
		{
			gold_flag = true;
			cout << "In cell " << current_cell << " is Gold! " << endl;
			cout << endl;
			cout << "Now agent is going back to the exit" << endl;
			cout << endl;
			bdd new_relation;
			while (current_cell != 0)
			{
				int count = 0;
				previous_cell.push(current_cell);
				reverse_cells[current_cell] = true;
				new_relation = R & q[current_cell];
				vector<int> neighb = neighbourNodes(current_cell);
				for (int i : neighb)
				{
					count++;
					if (safe_cells[i] == true && reverse_cells[i] == false)
					{
						new_relation &= qq[i];
						cell_to_go = i;
						find_path(R, q[current_cell], q, qq, qq[cell_to_go], answer, visited, direction);
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
								current_cell = move(current_cell, action, direction);
								cout << "action done by an agent - " << action << endl;
								new_plan.pop();
							}
						}
						current_cell = cell_to_go;
						cout << "Current cell is " << current_cell << endl;
						count--;
						break;
					}
				}
				if (neighb.size() == count)
				{
					current_cell = previous_cell.top();
					previous_cell.pop();
					current_cell = previous_cell.top();
					previous_cell.pop();
					cout << "Current cell is " << current_cell << endl;
				}
			}
			break;
		}

		task &= percept;

		cell_to_go = Enviroment(task, current_cell);

		bdd current_relation = R & q[current_cell];

		current_relation &= qq[cell_to_go];

		relation |= current_relation;

		find_path(current_relation, q[current_cell], q, qq, qq[cell_to_go], answer, visited, direction);
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
				current_cell = move(current_cell, action, direction);
				cout << "action done by an agent - " << action << endl;
				new_plan.pop();
			}
		}
		cells_visited_by_agent.push_back(current_cell);
		cout << "Current cell is " << current_cell << endl;

	} while (gold_flag != true);

    // ����� ������ ���������
	auto diff = std::chrono::high_resolution_clock::now() - start; 
	auto nsec = std::chrono::duration_cast<std::chrono::nanoseconds>(diff);
	cout << "It took: " << nsec.count() << " nanoseconds" << endl;

    // ����������
    bdd_done();
    system("pause");
    return 0;
}

// ����������� ������� ������������ ������ cell
vector<int> neighbourNodes(int cell)
{
    vector <int> neighbours;

    // ���� ������ �� � ������� ����� �������
    if (cell % N_COLUMN != 0)
    {
        neighbours.push_back(cell - 1);
    }
    // ���� ������ �� � ������� ������ �������
    if ((cell + 1) % N_COLUMN != 0)
    {
        neighbours.push_back(cell + 1);
    }
    // ���� ������ �� �� ����� ������ ������
    if (cell < N_COLUMN * (N_ROW - 1))
    {
        neighbours.push_back(cell + N_COLUMN);
    }
    // ���� ������ �� �� ����� ������� ������
    if (cell >= N_COLUMN)
    {
        neighbours.push_back(cell - N_COLUMN);
    }

    return neighbours;
}

int check_for_safety(bdd task, int current_cell) //�������� ������ �� ������������
{
	int cell_to_go_next;
	if ((task & !OK[current_cell]) == bddfalse) //���� ��� ���������
	{
		cell_to_go_next = current_cell; //������, ��� ���� ����� � ��� ������
		checked_cells[current_cell] = true; //������, ��� ��������� ��� ������
		safe_cells[current_cell] = true; //������, ��� ��� ������ ���������
		flag = true;
	}
	else if ((task & OK[current_cell]) == bddfalse) //������
	{
		not_safe_cells[current_cell] = true; //�������, ��� ��� ������ ������
		checked_cells[current_cell] = true;
		cell_to_go_next = cells->top();
		if ((task &= !P[current_cell]) == bddfalse)
			cout << "In cell " << current_cell << " is Pit!" << endl;
		else if ((task &= !W[current_cell]) == bddfalse)
			cout << "In cell " << current_cell << " is Wumpus!" << endl;
	}
	else //���� ���������
	{
		unknown_cells[current_cell] = true; //�������, ��� �����������
		//checked_cells[current_cell] = true; //��������� � ������ �����������
		cell_to_go_next = cells->top();
	}
	return cell_to_go_next;
}

int find_unvisited(bdd task, int current_cell) //��������� �� ������� ������, ������ ��, ������� ��� �� ���������, ��������� �� �� ��������
{
	vector <int> neighbours_of_cell = neighbourNodes(current_cell);

	for (int i = 0; i < neighbours_of_cell.size(); i++)
	{
		int check_if_safe = neighbours_of_cell[i];
		if (checked_cells[check_if_safe] == false)
		{
			cells->push(current_cell);
			current_cell = check_if_safe;
			break;
		}
	}
	return current_cell;
}

bdd ask_and_send_percept(vector<vector <int>> Enviroment, int current_cell) // � ������� ������ ���� percept
{
	{
		bdd percept = bddtrue;
		for (int i = 0; i < Enviroment[current_cell].size(); i++)
		{
			if (Enviroment[current_cell][i] == 22)
				percept &= S[current_cell];
			else if (Enviroment[current_cell][i] == 33)
				percept &= B[current_cell];
			else if (Enviroment[current_cell][i] == 4)
				percept &= G[current_cell];
			else percept &= !B[current_cell] & !S[current_cell] & !G[current_cell];
		}
		return percept;
	}
}

int Enviroment(bdd task, int current_cell)
{
	int cell_to_check;
	int got_unsafe_cell;
	int count = 0;

	cell_to_check = find_unvisited(task, current_cell); //��������� ������������ ������, ������� ����� ��������� �� ������������
	cell_to_check = check_for_safety(task, cell_to_check); //�������� �� ������������

	if (cell_to_check == current_cell)
	{
		vector<int> neighb = neighbourNodes(current_cell);
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
					return cell_to_check = check_for_safety(task, neighb[i]);
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

int move(int cell, string action, bdd &direction) //������� ��� ������������
{
	if (direction == n)
	{
		if (action == "forward")
			cell = cell - N_ROW;
		else if (action == "turnLeft")
			direction = w;
		else if (action == "turnRight")
			direction = e;
	}
	else if (direction == s)
	{
		if (action == "forward")
			cell = cell + N_ROW;
		else if (action == "turnLeft")
			direction = e;
		else if (action == "turnRight")
			direction = w;
	}
	else if (direction == e)
	{
		if (action == "forward")
			cell = cell + 1;
		else if (action == "turnLeft")
			direction = n;
		else if (action == "turnRight")
			direction = s;
	}
	else if (direction == w)
	{
		if (action == "forward")
			cell = cell -1;
		else if (action == "turnLeft")
			direction = s;
		else if (action == "turnRight")
			direction = n;
	}
	return cell;
}

void find_path(bdd relation, bdd first_state, bdd* q, bdd* qq, bdd wished_state, vector<int> answer, vector<bdd> visited, bdd &direction)
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
			if ((X_R & qq[i]) != bddfalse && qq[i] == wished_state)
			{
				answer.push_back(i);
				dostijima = true;
				flag = true;
				visited.push_back(qq[i]);
				if (direction == n)
					dir = 1;
				else if (direction == s)
					dir = 2;
				else if (direction == e)
					dir = 3;
				else if (direction == w)
					dir = 4;
				switch (dir)
				{
				case 1: //�����
				{
					if ((X_R & qq[i] & direction) != bddfalse)
					{
						str_plan.push("forward");
						plan.push(Forward_next);
					}

					else if ((X_R & qq[i] & direction) == bddfalse)
					{
						if ((X_R & qq[i] & e) != bddfalse)
						{
							str_plan.push("turnRight");
							plan.push(TurnRight_next);
							str_plan.push("forward");
							plan.push(Forward_next);
							//direction = e;
						}
						else if ((X_R & qq[i] & w) != bddfalse)
						{
							str_plan.push("turnLeft");
							plan.push(TurnLeft_next);
							str_plan.push("forward");
							plan.push(Forward_next);
							//direction = w;
						}
						else if ((X_R & qq[i] & s) != bddfalse)
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
				case 2: //��
				{
					if ((X_R & qq[i] & direction) != bddfalse)
					{
						str_plan.push("forward");
						plan.push(Forward_next);
					}
					else if ((X_R & qq[i] & direction) == bddfalse)
					{
						if ((X_R & qq[i] & e) != bddfalse)
						{
							str_plan.push("turnLeft");
							plan.push(TurnLeft_next);
							str_plan.push("forward");
							plan.push(Forward_next);
							//direction = e;
						}
						else if ((X_R & qq[i] & w) != bddfalse)
						{
							str_plan.push("turnRight");
							plan.push(TurnRight_next);
							str_plan.push("forward");
							plan.push(Forward_next);
							//direction = w;
						}
						else if ((X_R & qq[i] & n) != bddfalse)
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
				case 3: //������
				{
					if ((X_R & qq[i] & direction) != bddfalse)
					{
						str_plan.push("forward");
						plan.push(Forward_next);
					}
					else if ((X_R & qq[i] & direction) == bddfalse)
					{
						if ((X_R & qq[i] & s) != bddfalse)
						{
							str_plan.push("turnRight");
							plan.push(TurnRight_next);
							str_plan.push("forward");
							plan.push(Forward_next);
							//direction = s;
						}
						else if ((X_R & qq[i] & n) != bddfalse)
						{
							str_plan.push("turnLeft");
							plan.push(TurnLeft_next);
							str_plan.push("forward");
							plan.push(Forward_next);
							//direction = n;
						}
						else if ((X_R & qq[i] & w) != bddfalse)
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
				case 4: //�����
				{
					if ((X_R & qq[i] & direction) != bddfalse)
					{
						str_plan.push("forward");
						plan.push(Forward_next);
					}
					else if ((X_R & qq[i] & direction) == bddfalse)
					{
						if ((X_R & qq[i] & s) != bddfalse)
						{
							str_plan.push("turnLeft");
							plan.push(TurnLeft_next);
							str_plan.push("forward");
							plan.push(Forward_next);
							//direction = s;
						}
						else if ((X_R & qq[i] & n) != bddfalse)
						{
							str_plan.push("turnRight");
							plan.push(TurnRight_next);
							str_plan.push("forward");
							plan.push(Forward_next);
							//direction = n;
						}
						else if ((X_R & qq[i] & e) != bddfalse)
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
				//	cout << answer[k];
				//	out << answer[k];
				//}
				//out << endl;
				//out.close();
				//cout << endl;
				//break;
			}
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
					if (direction == n)
						dir = 1;
					else if (direction == s)
						dir = 2;
					else if (direction == e)
						dir = 3;
					else if (direction == w)
						dir = 4;
					switch (dir)
					{
					case 1: //�����
					{
						if ((X_R & qq[i] & direction) != bddfalse)
						{
							str_plan.push("forward");
							plan.push(Forward_next);
						}

						else if ((X_R & qq[i] & direction) == bddfalse)
						{
							if ((X_R & qq[i] & e) != bddfalse)
							{
								str_plan.push("turnRight");
								plan.push(TurnRight_next);
								str_plan.push("forward");
								plan.push(Forward_next);
								//direction = e;
							}
							else if ((X_R & qq[i] & w) != bddfalse)
							{
								str_plan.push("turnLeft");
								plan.push(TurnLeft_next);
								str_plan.push("forward");
								plan.push(Forward_next);
								//direction = w;
							}
							else if ((X_R & qq[i] & s) != bddfalse)
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
					case 2: //��
					{
						if ((X_R & qq[i] & direction) != bddfalse)
						{
							str_plan.push("forward");
							plan.push(Forward_next);
						}
						else if ((X_R & qq[i] & direction) == bddfalse)
						{
							if ((X_R & qq[i] & e) != bddfalse)
							{
								str_plan.push("turnLeft");
								plan.push(TurnLeft_next);
								str_plan.push("forward");
								plan.push(Forward_next);
								//direction = e;
							}
							else if ((X_R & qq[i] & w) != bddfalse)
							{
								str_plan.push("turnRight");
								plan.push(TurnRight_next);
								str_plan.push("forward");
								plan.push(Forward_next);
								//direction = w;
							}
							else if ((X_R & qq[i] & n) != bddfalse)
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
					case 3: //������
					{
						if ((X_R & qq[i] & direction) != bddfalse)
						{
							str_plan.push("forward");
							plan.push(Forward_next);
						}
						else if ((X_R & qq[i] & direction) == bddfalse)
						{
							if ((X_R & qq[i] & s) != bddfalse)
							{
								str_plan.push("turnRight");
								plan.push(TurnRight_next);
								str_plan.push("forward");
								plan.push(Forward_next);
								//direction = s;
							}
							else if ((X_R & qq[i] & n) != bddfalse)
							{
								str_plan.push("turnLeft");
								plan.push(TurnLeft_next);
								str_plan.push("forward");
								plan.push(Forward_next);
								//direction = n;
							}
							else if ((X_R & qq[i] & w) != bddfalse)
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
					case 4: //�����
					{
						if ((X_R & qq[i] & direction) != bddfalse)
						{
							str_plan.push("forward");
							plan.push(Forward_next);
						}
						else if ((X_R & qq[i] & direction) == bddfalse)
						{
							if ((X_R & qq[i] & s) != bddfalse)
							{
								str_plan.push("turnLeft");
								plan.push(TurnLeft_next);
								str_plan.push("forward");
								plan.push(Forward_next);
								//direction = s;
							}
							else if ((X_R & qq[i] & n) != bddfalse)
							{
								str_plan.push("turnRight");
								plan.push(TurnRight_next);
								str_plan.push("forward");
								plan.push(Forward_next);
								//direction = n;
							}
							else if ((X_R & qq[i] & e) != bddfalse)
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
								find_path(relation, q[i], q, qq, wished_state, answer, visited, direction);
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
		else X_pr = first_state;
	}
}

