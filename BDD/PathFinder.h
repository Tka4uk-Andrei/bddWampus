#pragma once

#pragma comment(lib,"bdd.lib")
#pragma warning (disable : 26444)

#include "bdd.h"

#include <vector>
#include <stack>
#include <string>

using namespace std;

bool to_stop_recursion;
stack <string> str_plan;


void find_path(stack <bdd>& plan, stack <string>& str_plan, bdd relation, bdd first_state, bdd* q, bdd* qq, bdd wished_state, vector<int> answer, vector<bdd> visited, bdd& direction)
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
                case 1: //север
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
                case 2: //юг
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
                case 3: //восток
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
                case 4: //запад
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
                    case 1: //север
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
                    case 2: //юг
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
                    case 3: //восток
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
                    case 4: //запад
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
                                find_path(plan, str_plan, relation, q[i], q, qq, wished_state, answer, visited, direction);
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

void bddAstar()
{

}
