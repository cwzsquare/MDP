// lab4.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>
#include <algorithm>
#include <string>
using namespace std;

const double Punishment = -100;
const double Reward = 100;
const double gamma = 0.9;
const double epsilon = 1e-6;

// 新建动作-转移概率矩阵
// 四块儿分别对应，上，下，左，右。即P = P[a][s][s']
// 上:0，下:1，左:2，右:3
const double P[4][11][11] = {
    {{0.15, 0.8, 0, 0.05, 0, 0, 0, 0, 0, 0, 0},
     {0, 0.2, 0.8, 0, 0, 0, 0, 0, 0, 0, 0},
     {0, 0, 0.95, 0, 0.05, 0, 0, 0, 0, 0, 0},
     {0.05, 0, 0, 0.9, 0, 0.05, 0, 0, 0, 0, 0},
     {0, 0, 0.05, 0, 0.9, 0, 0, 0.05, 0, 0, 0},
     {0, 0, 0, 0.05, 0, 0.1, 0.8, 0, 0.05, 0, 0},
     {0, 0, 0, 0, 0, 0, 0.15, 0.8, 0, 0.05, 0},
     {0, 0, 0, 0, 0.05, 0, 0, 0.9, 0, 0, 0.05},
     {0, 0, 0, 0, 0, 0.05, 0, 0, 0.15, 0.8, 0},
     {0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0},
     {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1}},

    {{0.95, 0, 0, 0.05, 0, 0, 0, 0, 0, 0, 0},
     {0.8, 0.2, 0, 0, 0, 0, 0, 0, 0, 0, 0},
     {0, 0.8, 0.15, 0, 0.05, 0, 0, 0, 0, 0, 0},
     {0.05, 0, 0, 0.9, 0, 0.05, 0, 0, 0, 0, 0},
     {0, 0, 0.05, 0, 0.9, 0, 0, 0.05, 0, 0, 0},
     {0, 0, 0, 0.05, 0, 0.9, 0, 0, 0.05, 0, 0},
     {0, 0, 0, 0, 0, 0.8, 0.15, 0, 0, 0.05, 0},
     {0, 0, 0, 0, 0.05, 0, 0.8, 0.1, 0, 0, 0.05},
     {0, 0, 0, 0, 0, 0.05, 0, 0, 0.95, 0, 0},
     {0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0},
     {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1}},

    {{0.95, 0.05, 0, 0, 0, 0, 0, 0, 0, 0, 0},
     {0.05, 0.9, 0.05, 0, 0, 0, 0, 0, 0, 0, 0},
     {0, 0.05, 0.95, 0, 0, 0, 0, 0, 0, 0, 0},
     {0.8, 0, 0, 0.2, 0, 0, 0, 0, 0, 0, 0},
     {0, 0, 0.8, 0, 0.2, 0, 0, 0, 0, 0, 0},
     {0, 0, 0, 0.8, 0, 0.15, 0.05, 0, 0, 0, 0},
     {0, 0, 0, 0, 0, 0.05, 0.9, 0.05, 0, 0, 0},
     {0, 0, 0, 0, 0.8, 0, 0.05, 0.15, 0, 0, 0},
     {0, 0, 0, 0, 0, 0.8, 0, 0, 0.15, 0.05, 0},
     {0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0},
     {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1}},

    {{0.15, 0.05, 0, 0.8, 0, 0, 0, 0, 0, 0, 0},
     {0.05, 0.9, 0.05, 0, 0, 0, 0, 0, 0, 0, 0},
     {0, 0.05, 0.15, 0, 0.8, 0, 0, 0, 0, 0, 0},
     {0, 0, 0, 0.2, 0, 0.8, 0, 0, 0, 0, 0},
     {0, 0, 0, 0, 0.2, 0, 0, 0.8, 0, 0, 0},
     {0, 0, 0, 0, 0, 0.15, 0.05, 0, 0.8, 0, 0},
     {0, 0, 0, 0, 0, 0.05, 0.1, 0.05, 0, 0.8, 0},
     {0, 0, 0, 0, 0, 0, 0.05, 0.15, 0, 0, 0.8},
     {0, 0, 0, 0, 0, 0, 0, 0, 0.95, 0.05, 0},
     {0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0},
     {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1}},
};

double calExpect(const double *A)
{
    return A[10] * Reward + A[9] * Punishment;
}

double vectorMul(const double *A, double *B)
{
    double sum = 0;
    for (int i = 0; i < 11; i++)
    {
        sum += A[i] * B[i];
    }
    return sum;
}

//一个有趣的argmax，按照原来max默认为0，S9会一头创死
int argmax(double *Q)
{
    int index = 0;
    double max = Q[0];
    for (int i = 1; i < 4; i++)
    {
        if (Q[i] > max)
        {
            max = Q[i];
            index = i;
        }
    }
    return index;
}

// 将action_value翻译成汉字
string translate(int index)
{
    switch (index)
    {
    case 0:
        return string("上");
    case 1:
        return string("下");
    case 2:
        return string("左");
    default:
        return string("右");
    }
}

//判断V是否收敛
bool is_astringent(double *V, double *V_last)
{
    double diff = 0;
    for (int i = 0; i < 11; i++)
    {
        diff += abs(V[i] - V_last[i]);
    }
    if (diff < epsilon)
        return true;
    return false;
}

//传入Q[s]，返回最大的Value
double maxaQ(double *Q)
{
    double maxV = Q[0];
    for (int i = 1; i < 4; i++)
    {
        maxV = max(maxV, Q[i]);
    }
    return maxV;
}

int main()
{
    /* 值迭代 */
    double V[11] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    double Q[11][4] = {0};

    /*请在此处补全值迭代的代码
    * 
    */
    double V_last[11] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    bool flag = false;

    while (true)
    {
        flag = false;

        for (int i = 0; i < 11; i++)
        {
            V_last[i] = V[i];
        }

        for (int s = 0; s < 11; s++)
        {
            for (int a = 0; a < 4; a++)
            {
                Q[s][a] = calExpect(P[a][s]) + gamma * vectorMul(P[a][s], V);
            }

            V[s] = maxaQ(Q[s]);
        }

        flag = is_astringent(V, V_last);
        if (flag)
            break;
    }

    cout << "Reward = " << Reward << " , "
         << " Punishment = " << Punishment << endl;
    cout << "值迭代： " << endl;
    cout << "状态"
         << "\t"
         << " "
         << "\t"
         << "价值" << endl;
    for (int s = 0; s < 11; s++)
    {
        cout << s + 1 << "\t"
             << " : "
             << "\t" << V[s] << endl;
    }

    cout << "---------------------------------------------" << endl;

    //Print Policy
    cout << "状态"
         << "\t"
         << " "
         << "\t"
         << "动作" << endl;
    for (int s = 0; s < 9; s++)
    {
        cout << s + 1 << "\t"
             << " -> "
             << "\t" << translate(argmax(Q[s])) << endl;
    }

    cout << "---------------------------------------------" << endl;

    /* 策略迭代 */
    int pi[11] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    double V_2[11] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

    /* 请在此处补全策略迭代的代码
    *
    */
    double delta = 0;
    bool policy_stable = true;
    double A[4] = {0}; //action数组，用作传给argmax输出最佳策略
    int tempi = 0;
    double tempv = 0;
    while (true)
    {

        //Policy Evaluation
        while (true)
        {
            delta = 0;
            for (int s = 0; s < 11; s++)
            {
                tempv = V_2[s];
                V_2[s] = calExpect(P[pi[s]][s]) + gamma * vectorMul(P[pi[s]][s], V_2);
                delta = max(delta, abs(tempv - V_2[s]));
            }

            if (delta < epsilon)
                break;
        }

        //Policy Improvement
        policy_stable = true;
        for (int s = 0; s < 11; s++)
        {
            tempi = pi[s];

            for (int a = 0; a < 4; a++)
            {
                A[a] = calExpect(P[a][s]) + gamma * vectorMul(P[a][s], V_2);
            }
            pi[s] = argmax(A);

            if (tempi != pi[s])
                policy_stable = false;
        }
        if (policy_stable)
            break;
    }

    cout << "策略迭代： " << endl;
    cout << "状态"
         << "\t"
         << " "
         << "\t"
         << "价值" << endl;
    for (int s = 0; s < 11; s++)
    {
        cout << s + 1 << "\t"
             << " : "
             << "\t" << V_2[s] << endl;
    }

    cout << "---------------------------------------------" << endl;
    cout << "状态"
         << "\t"
         << " "
         << "\t"
         << "动作" << endl;
    for (int s = 0; s < 9; s++)
    {
        cout << s + 1 << "\t"
             << " -> "
             << "\t" << translate(pi[s]) << endl;
    }

    return 0;
}
