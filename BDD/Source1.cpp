#pragma comment(lib,"bdd.lib")
#pragma warning (disable : 26444)
#include"bdd.h"
#include<fstream>
#include <iostream>
#include <vector>
#include <cmath>
#include <time.h>
#include <map>

///////////////////////////////// �������� �� �������� ��� ���������� ��� �������� ����������� ��� ������?? ���������� ////////////////////////////////////////

using namespace std;

ofstream out;

//N_DEPEND_ON_CELL*N_COLUMN*N_ROW+N_NOT_DEPEND_ON_CELL+18
#define N_DEPEND_ON_CELL 9
#define N_NOT_DEPEND_ON_CELL 18
#define N_COLUMN 1
#define N_ROW 1
#define N N_COLUMN*N_ROW
#define N_VAR 50 // ����� ������� ���������� (220?)
void fun(char* varset, int size); //�������, ������������ ��� ������ �������
void print(); // ������ � ����


//���������� Stench � Breeze [2], ��� ��� ��� ������, ������� ��������� ���������� � �������

//���������� ���������� �����
bdd Stench[2];
bdd Breeze[2];
bdd Scream[2];

// ���������� ��������� �����, ���������, �� ������� �� �������
bdd B[N_COLUMN*N_ROW]; //�����
bdd P[N_COLUMN*N_ROW]; //���
bdd S[N_COLUMN*N_ROW]; //�����
bdd W[N_COLUMN*N_ROW]; //������


//���������� ��������� �����, ������� �� �������
bdd HaveArrow[2];//����� ������ 
bdd HaveArrow_next[2]; //����� ������ � ��������� ���������

bdd WumpusAlive[2]; //������ ��� 
bdd WumpusAlive_next[2];

bdd HaveGold[2]; //����� ������ 
bdd HaveGold_next[2];

bdd ClimbedOut[2]; //������� �� ������ 
bdd ClimbedOut_next[2];

//���������� ��������� ������, ������� �� �������
bdd L[N]; //���������� ������
bdd L_next[N];

bdd North[2]; //�������� �� �����
bdd North_next[2];

bdd West[2];
bdd West_next[2];

bdd South[2];
bdd South_next[2];

bdd East[2];
bdd East_next[2];

bdd V[N]; //�������� �������
bdd V_next[N];

bdd OK[N]; //���������� ������
bdd OK_next[N];

//���������� ��������, ������� �� �������
bdd Forward[2];
bdd Forward_next[2];

bdd TurnRight[2];
bdd TurnRight_next[2];

bdd TurnLeft[2];
bdd TurnLeft_next[2];

bdd Grab[2];
bdd Grab_next[2];

bdd Climb[2];
bdd Climb_next[2];

bdd Shoot[2];
bdd Shoot_next[2];

//������� ������������������� �������, ����� ��������
int main() {
	int countvar = 0;
	bdd_init(10000000, 1000000);//�������� ������ ��� 1000000 ����� ������� � ��� �������� 100000
	bdd_setvarnum(N_VAR); //������ ���������� ������� ����������

	bdd task = bddtrue; //�������. ���������� true. ����� ����� ���������� ����
	
	//////////////////////////////////// � � � � � � � ////////////////////////////////////////////////

	//��� �������
	for (int i = 0; i < N; i++)
	{
		W[i] = bdd_ithvar(countvar);
		countvar++;
		//(i == 8) ? W[i] = bdd_ithvar(i) : W[i] = bdd_nithvar(i); //������� �� �����, � ����� � ������ ������� �� ��������� W[i]
	}

	//��� ������
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

	//��� Stench

	Stench[0] = bdd_ithvar(countvar); //��������� ���������� ������
	countvar++;
	Stench[1] = bdd_ithvar(countvar); //������������� �������� ���������� ������
	countvar++;

	//��� Breeze

	Breeze[0] = bdd_ithvar(countvar); //��������� ���������� �����
	countvar++;
	Breeze[1] = bdd_ithvar(countvar);  //������������� �������� ���������� �����
	countvar++;

	//��� Scream
	Scream[0] = bdd_ithvar(countvar);
	countvar++;
	Scream[1] = bdd_ithvar(countvar);
	countvar++;


	///////////////////////////////////////////////� � � � � � � �//////////////////////////////////////////////////////

	//��� ���������� ������ � ������
	for (int i=0; i<N; i++)
	{
		L[i] = bdd_ithvar(countvar);
		countvar++;
	}

	//��� ��������� ������ �������
	for (int i=0; i<N; i++)
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
	HaveArrow[0] = bdd_ithvar(countvar); //��� ������
	countvar++;
	HaveArrow[1] = bdd_ithvar(countvar); //���� ������
	countvar++;

	//��� ���� ��� ������ ���
	WumpusAlive[0] = bdd_ithvar(countvar);
	countvar++;
	WumpusAlive[1] = bdd_ithvar(countvar);
	countvar++;

	//��� �����������
	East[0] = bdd_ithvar(countvar);
	countvar++;
	East[1] = bdd_ithvar(countvar);
	countvar++;

	South[0] = bdd_ithvar(countvar);
	countvar++;
	South[1] = bdd_ithvar(countvar);
	countvar++;

	West[0] = bdd_ithvar(countvar);
	countvar++;
	West[1] = bdd_ithvar(countvar);
	countvar++;

	North[0] = bdd_ithvar(countvar);
	countvar++;
	North[1] = bdd_ithvar(countvar);
	countvar++;

	//��� �������� (�������, �������, �������)
	Forward[0] = bdd_ithvar(countvar);
	countvar++;
	Forward[1] = bdd_ithvar(countvar);
	countvar++;

	TurnLeft[0] = bdd_ithvar(countvar);
	countvar++;
	TurnLeft[1] = bdd_ithvar(countvar);
	countvar++;

	TurnRight[0] = bdd_ithvar(countvar);
	countvar++;
	TurnRight[1] = bdd_ithvar(countvar);
	countvar++;

	Grab[0] = bdd_ithvar(countvar);
	countvar++;
	Grab[1] = bdd_ithvar(countvar);
	countvar++;

	Climb[0] = bdd_ithvar(countvar);
	countvar++;
	Climb[1] = bdd_ithvar(countvar);
	countvar++;

	Shoot[0] = bdd_ithvar(countvar);
	countvar++;
	Shoot[1] = bdd_ithvar(countvar);
	countvar++;

	//��� ��������� ������ (������, ����� �� �� ������?)

	ClimbedOut[0] = bdd_ithvar(countvar);
	countvar++;
	ClimbedOut[1] = bdd_ithvar(countvar);
	countvar++;

	HaveGold[0] = bdd_ithvar(countvar);
	countvar++;
	HaveGold[1] = bdd_ithvar(countvar);
	countvar++;

	////////////////////////////////////////���������� ��� ���������� ���������///////////////////////////////////////

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

	HaveArrow_next[0] = bdd_ithvar(countvar); //��� ������
	countvar++;
	HaveArrow_next[1] = bdd_ithvar(countvar); //���� ������
	countvar++;

	//��� ���� ��� ������ ���
	WumpusAlive_next[0] = bdd_ithvar(countvar);
	countvar++;
	WumpusAlive_next[1] = bdd_ithvar(countvar);
	countvar++;

	//��� �����������
	East_next[0] = bdd_ithvar(countvar);
	countvar++;
	East_next[1] = bdd_ithvar(countvar);
	countvar++;

	South_next[0] = bdd_ithvar(countvar);
	countvar++;
	South_next[1] = bdd_ithvar(countvar);
	countvar++;

	West_next[0] = bdd_ithvar(countvar);
	countvar++;
	West_next[1] = bdd_ithvar(countvar);
	countvar++;

	North_next[0] = bdd_ithvar(countvar);
	countvar++;
	North_next[1] = bdd_ithvar(countvar);
	countvar++;

	//��� �������� (�������, �������, �������)
	Forward_next[0] = bdd_ithvar(countvar);
	countvar++;
	Forward_next[1] = bdd_ithvar(countvar);
	countvar++;

	TurnLeft_next[0] = bdd_ithvar(countvar);
	countvar++;
	TurnLeft_next[1] = bdd_ithvar(countvar);
	countvar++;

	TurnRight_next[0] = bdd_ithvar(countvar);
	countvar++;
	TurnRight_next[1] = bdd_ithvar(countvar);
	countvar++;

	Grab_next[0] = bdd_ithvar(countvar);
	countvar++;
	Grab_next[1] = bdd_ithvar(countvar);
	countvar++;

	Climb_next[0] = bdd_ithvar(countvar);
	countvar++;
	Climb_next[1] = bdd_ithvar(countvar);
	countvar++;

	Shoot_next[0] = bdd_ithvar(countvar);
	countvar++;
	Shoot_next[1] = bdd_ithvar(countvar);
	countvar++;

	//��� ��������� ������ (������, ����� �� �� ������?)

	ClimbedOut_next[0] = bdd_ithvar(countvar);
	countvar++;
	ClimbedOut_next[1] = bdd_ithvar(countvar);
	countvar++;

	HaveGold_next[0] = bdd_ithvar(countvar);
	countvar++;
	HaveGold_next[1] = bdd_ithvar(countvar);
	countvar++;


	//////////////////////////////////////////////////////////////// � � � � � � � � �   � � � � � //////////////////////////////////////////////////////////////////


	std::vector <int> real_cave(16);

	int* temp_pit = new int [3];//��� ��
	int temp_w;
	int temp_g;
	int* index_pit = new int [3];
	int index_w;
	int index_g;
	srand(time(NULL));
	int k = (int)(0.2 * N);
	int nums[15] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15 };

	/////////////////////////////////////////////////////////// � � � � � � � � �   � �,   � � � � � � �,   � � � � � � ///////////////////////////////////////////////////////////////////////////////////
	index_pit[0] = rand() % (N - 1);
	real_cave[index_pit[0]] = 3;
	for (int i = 1; i < k; i++)
	{

		for (int j = 1; j < k; j++)
		{
			index_pit[j] = index_pit[i-1];
			if (index_pit[i] != index_pit[j])
			{
				temp_pit[i] = nums[index_pit[i]];
			}
			else index_pit[i] = rand() % (N - 1);
		}
			real_cave[temp_pit[i]] = 3;
	}
	index_w = rand() % (N - 1);
	for (int i = 0; i < k; i++)
	{
		if (index_w == index_pit[i])
			index_w = rand() % (N - 1);
		else
		{
			temp_w = nums[index_w];
			real_cave[temp_w] = 2;
		}
	}
	index_g = rand() % (N - 1);
	for (int i = 0; i < k+1; i++)
	{
		if (index_g == index_pit[i] || index_g == index_w)
			index_g = rand() % (N - 1);
		else
		{
			temp_g = nums[index_g];
			real_cave[temp_g] = 4;
		}
	}
	int k1 = 0;
	real_cave[0] = 1;

	cout << " 1 - AGENT \n 2 - WUMPUS \n 3 - PIT \n 4 - GOLD \n 22 - STENCH \n 33 - BREEZE \n ";
	cout << "The real CAVE is:" << "\n";
	
	for (int i = 0; i < N; i++)
	{
		cout << real_cave[i] << "   ";
		k1++;
		if (k1 % N_COLUMN == 0)
			cout << "\n";
	}


	//cout << countvar;


	//��������� ������� � ���� (� ����� �� ��������� ���� ��, ��� �� � 8�� ������ ��� ����� �������� ������ ��, ��� ��� ��� � 1?
	//task &= W[8];

	// ��������� �� ������
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

	task &= HaveArrow[1]; //���� ������
	task &= WumpusAlive[1];

	task &= East[1];
	task &= !West[0];
	task &= !South[0];
	task &= !North[0];

	task &= !Forward[0];
	task &= !TurnLeft[0];
	task &= !TurnRight[0];

	task &= !Grab[0];
	task &= !Shoot[0];
	task &= !Climb[0];

	task &= !ClimbedOut[0];
	task &= !HaveGold[0];

	//���� ������ (������� ������)
	for (int i = 0; i < N; i++) //��������� (^ ��� &???)
	{
		if ((i % N_COLUMN == 0) && (i/N_ROW == 0)) //��� 0
		{
			task &= (!(B[i] ^ P[i + 1]) & !(B[i] ^ P[i + N_COLUMN]));
			task &= (!(S[i] ^ W[i + 1]) & !(S[i] ^ W[i + N_COLUMN]));
		}
		else if (i % N_COLUMN == N_COLUMN-1) //��� 1 � 2
		{
			task &= (!(B[i] ^ P[i - 1]) & !(B[i] ^ P[i + N_COLUMN]) & !(B[i] ^ P[i + 1]));
			task &= (!(S[i] ^ W[i - 1]) & !(S[i] ^ W[i + N_COLUMN]) & !(S[i] ^ W[i + 1]));
		}
		else if (i % N_COLUMN == 0) //��� 4 � 8
		{
			task &= (!(B[i] ^ P[i + 1]) & !(B[i] ^ P[i + N_COLUMN]) & !(B[i] ^ P[i - N_COLUMN]));
			task &= (!(S[i] ^ W[i + 1]) & !(S[i] ^ W[i + N_COLUMN]) & !(S[i] ^ W[i - N_COLUMN]));
		}
		else if ((i % N_COLUMN == 0) && (i / N_ROW == N_ROW-1)) //��� 12
		{
			task &= (!(B[i] ^ P[i + 1]) & !(B[i] ^ P[i - N_COLUMN]));
			task &= (!(S[i] ^ W[i + 1]) & !(S[i] ^ W[i - N_COLUMN]));
		}
		else if (i / N_ROW == N_ROW - 1) //��� 13 � 14
		{
			task &= (!(B[i] ^ P[i + 1]) & !(B[i] ^ P[i - N_COLUMN]) & !(B[i] ^ P[i - 1]));
			task &= (!(S[i] ^ W[i + 1]) & !(S[i] ^ W[i - N_COLUMN]) & !(S[i] ^ W[i - 1]));
		}
		else if ((i / N_ROW == N_ROW - 1) && (i % N_COLUMN == N_COLUMN - 1)) //��� 15
		{
			task &= (!(B[i] ^ P[i - 1]) & !(B[i] ^ P[i - N_COLUMN]));
			task &= (!(S[i] ^ W[i - 1]) & !(S[i] ^ W[i - N_COLUMN]));
		}
		else if ((i / N_ROW == 0) && (i % N_COLUMN == N_COLUMN - 1)) // ��� 3
		{
			task &= (!(B[i] ^ P[i - 1]) & !(B[i] ^ P[i + N_COLUMN]));
			task &= (!(S[i] ^ W[i - 1]) & !(S[i] ^ W[i + N_COLUMN]));
		}
		else if (i % N_COLUMN == N_COLUMN - 1) //��� 7 � 11
		{
			task &= (!(B[i] ^ P[i + N_COLUMN]) & !(B[i] ^ P[i - N_COLUMN]) & !(B[i] ^ P[i - 1]));
			task &= (!(S[i] ^ W[i + N_COLUMN]) & !(S[i] ^ W[i - N_COLUMN]) & !(S[i] ^ W[i - 1]));
		}
		else //��� 5, 6, 9, 10
		{
			task &= (!(B[i] ^ P[i + N_COLUMN]) & !(B[i] ^ P[i - N_COLUMN]) & !(B[i] ^ P[i - 1]) & !(B[i] ^ P[i + 1]));
			task &= (!(S[i] ^ W[i + N_COLUMN]) & !(S[i] ^ W[i - N_COLUMN]) & !(S[i] ^ W[i - 1]) & !(S[i] ^ W[i + 1]));
		}
	}

	//// ���� ���� �� ���� ������ ��� �������?

	//for (int i = 0; i < N; i++) //�� ��� �� ����� ������ �������           � � � � � � � � � 
	//{
	//	for (int j = 0; j < N; j++)
	//	{
	//		if (j != i)
	//			task &= !(W[i] ^ !W[j]);
	//	}
	//}

	task &= !(HaveArrow_next[1] ^ (HaveArrow[0] & !Shoot[0])); //���������� � ������ � ����������.............(�������� 0�� �������� ��� 1��?) ������� 0� �������, ��� ��� ��� ������� ��������
	task &= !(WumpusAlive_next[1] ^ (WumpusAlive[0] & !Scream[0]));

	//����������� ������
	task &= !(North_next[1] ^ ((TurnRight[0] & West[0]) | (TurnLeft[0] & East[0]) | (North[0] & !TurnLeft[0] & !TurnRight[0])));
	task &= !(South_next[1] ^ ((TurnRight[0] & East[0]) | (TurnLeft[0] & West[0]) | (South[0] & !TurnLeft[0] & !TurnRight[0])));
	task &= !(East_next[1] ^ ((TurnRight[0] & North[0]) | (TurnLeft[0] & South[0]) | (East[0] & !TurnLeft[0] & !TurnRight[0])));
	task &= !(West_next[1] ^ ((TurnRight[0] & South[0]) | (TurnLeft[0] & North[0]) | (West[0] & !TurnLeft[0] & !TurnRight[0])));

	task &= !(HaveGold_next[1] ^ (Grab[0] & HaveGold[0]));
	task &= !(ClimbedOut_next[1] ^ (Climb[0] & ClimbedOut[0])); //���������� ��������� ����������

	for (int i = 0; i < N; i++) //����� �� �������(1) ��� �� �������(0)???  � � � � � � � � � 
	{
		if (i % N_COLUMN == 1)
			task &= !(L_next[i] ^ ((L[i] & !Forward[1]) | (L[i-N_COLUMN] & North[0] & Forward[0]) | (L[i+1] & West[0] & Forward[0]) | (L[i+N_COLUMN] & South[0] & Forward[0])));
		else if (i % N_COLUMN == 0)
			task &= !(L_next[i] ^ ((L[i] & !Forward[0]) | (L[i - N_COLUMN] & North[0] & Forward[0]) | (L[i - 1] & East[0] & Forward[0]) | (L[i + N_COLUMN] & South[0] & Forward[0])));
		else
			task &= !(L_next[i] ^ ((L[i] & !Forward[0]) | (L[i - N_COLUMN] & North[0] & Forward[0]) | (L[i - 1] & East[0] & Forward[0]) | (L[i + 1] & West[0] & Forward[0]) | (L[i + N_COLUMN] & South[0] & Forward[0])));

		pair<bdd, bdd> for_swap;
		pair<bdd, bdd> swaped;
		for_swap.first = L[i];
		for_swap.second = L_next[i];
		swaped.first = for_swap.second;
		swaped.second = for_swap.first;
		L[i] = swaped.first;
	}

	for (int i = 0; i < N; i++)
	{
		task &= !(V_next[i] ^ (V[i]|L[i]));
	}

	for (int i = 0; i < N; i++) //���������� �� ������� ����� � ������ ����������
	{
		task &= (L[i] >> (!(Breeze[0] ^ B[i])));
		task &= (L[i] >> (!(Stench[0] ^ S[i])));
		task &= L[i] >> V[i];
		task &= !(OK[i] ^ (!P[i] & !(W[i] & WumpusAlive[0])));
	}

	//�������������� �������� � ���������� (����� ���� (������� �� 0 ��� �� 1?))

	task &= (Shoot[0] >> HaveArrow[0]);

	task &= (Forward[0] >> (!Shoot[0] & !TurnLeft[0] & !TurnRight[0] & !Grab[0] & !Climb[0]));
	task &= (Shoot[0] >> (!Forward[0] & !TurnLeft[0] & !TurnRight[0] & !Grab[0] & !Climb[0]));
	task &= (TurnLeft[0] >> (!Shoot[0] & !Forward[0] & !TurnRight[0] & !Grab[0] & !Climb[0]));
	task &= (TurnRight[0] >> (!Shoot[0] & !TurnLeft[0] & !Forward[0] & !Grab[0] & !Climb[0]));
	task &= (Grab[0] >> (!Shoot[0] & !TurnLeft[0] & !Forward[0] & !TurnRight[0] & !Climb[0]));
	task &= (Climb[0] >> (!Shoot[0] & !TurnLeft[0] & !Forward[0] & !Grab[0] & !TurnRight[0]));


	out.open("out.txt");
	unsigned int satcount = (int)bdd_satcount(task);// ���������� �������
	out << satcount << " solutions:\n" << endl;
	//if (satcount) bdd_allsat(task, fun);
	out.close();
	bdd_done();
	system("pause");
}

char var[N_VAR];

//void print() {
//	for (int i = 0; i < N_COLUMN; i++) {
//		out << i << ": ";
//		for (unsigned j = 0; j < N_ROW; j++) {
//			int J = i * N_COLUMN * N_ROW + j * N_ROW;
//			int num = 0;
//			for (unsigned k = 0; k < N_ROW; k++) num += (unsigned)
//				(var[J + k] << k);
//			out << num << ' ';
//		}
//		out << endl;
//	}
//	out << endl;
//}
void build(char* varset, int n, int I)
{
	if (I == n - 1) {
		if (varset[I] >= 0) {
			var[I] = varset[I];
			//print();
			return;
		}
		var[I] = 0;
		//print();
		var[I] = 1;
		//print();
		return;
	}
	if (varset[I] >= 0) {
		var[I] = varset[I];
		build(varset, n, I + 1);
		return;
	}
	var[I] = 0;
	build(varset, n, I + 1);
	var[I] = 1;
	build(varset, n, I + 1);
}
void fun(char* varset, int size) {
	build(varset, size, 0);
}