#include <iostream>
#include <vector>
#include <cmath>
#include <cstring>
using namespace std;
const int N = 100010;
const double eps = 1e-10;

// Implement your Gaussian Elimination algorithm.
// You can add any standard library if needed.
// ...

char str[N];
void parse(vector<vector<double>> &v, int &n, int &m)
{
    m = -1;
    while (cin.getline(str, 100))
    {
        v.push_back(vector<double>());
        m++;
        n = 0;
        double num = 0;
        double fnum = 1;
        bool wait_num = true;
        bool neg = false;
        for (int i = 0; str[i] != '\n' && str[i] != '\0'; i++)
        {
            if (str[i] == '-')
            {
                neg = true;
                i++;
            }
            while (str[i] >= '0' && str[i] <= '9')
            {
                wait_num = false;
                num = num * 10 + str[i] - '0';
                i++;
            }
            if (str[i] == '.')
            {
                i++;
                while (str[i] >= '0' && str[i] <= '9')
                {
                    fnum /= 10;
                    num += fnum * (str[i] - '0');
                    i++;
                }
            }
            if (!wait_num)
            {
                wait_num = true;
                if (neg)
                {
                    neg = !neg;
                    v[m].push_back(double(-num));
                }
                else
                {
                    v[m].push_back(double(num));
                }
                num = 0;
                fnum = 1;
                n++;
            }
        }
        memset(str, 0, 100);
    }
    m++;
}
bool e[N];
bool done[N];
double ans[N];
int main()
{
    // Input processing.
    // ...
    // Solve the linear system and print the results.
    // ...
    vector<vector<double>> a;
    int n = 0, m = 0;
    parse(a, n, m);
    int k;
    // for (int i = 0; i < m; ++i)
    //{
    // for (int j = 0; j < n; ++j)
    //{
    // cout << a[i][j] << ' ';
    //}
    // cout << endl;
    //}
    // cout << "n:" << n << "m:" << m << endl;
    // cout << "*********" << endl;
    for (k = 0; k < m && k < n - 1; ++k)
    {
        for (int h = 0; h < m; ++h)
        {
            if (abs(a[h][k]) <= eps || e[h])
                continue;
            e[h] = true;
            for (int i = 0; i < m; ++i)
            {
                if (e[i])
                    continue;
                double factor = a[i][k] / a[h][k];
                for (int j = k; j < n; ++j)
                    a[i][j] -= (a[h][j] * factor);
            }
        }
    }
    //    for (int i = 0; i < m; ++i)
    //{
    // for (int j = 0; j < n; ++j)
    //{
    // cout << a[i][j] << ' ';
    //}
    // cout << endl;
    //}
    // cout << "*********" << endl;
    for (int i = 0; i < m; ++i)
    {
        bool flag = false;
        for (int j = 0; j < n - 1; ++j)
        {
            if (abs(a[i][j]) >= eps)
            {
                flag = true;
                break;
            }
        }
        if (!flag)
        {
            if (abs(a[i][n - 1]) <= eps)
                cout << "No unique solution!" << endl;
            else
                cout << "No solution!" << endl;
            return 0;
        }
    }
    for (int i = 0; i < m; ++i)
        e[i] = false;
    for (k = k - 1; k >= 0; k--)
    {
        for (int h = 0; h < m; ++h)
        {
            bool flag = false;
            for (int i = 0; i < k; ++i)
            {
                if (abs(a[h][i]) >= eps)
                {
                    flag = true;
                    break;
                }
            }
            if (abs(a[h][k]) <= eps || e[h] || flag)
                continue;
            e[h] = true;
            for (int i = 0; i < m; ++i)
            {
                if (e[i])
                    continue;
                double factor = a[i][k] / a[h][k];
                for (int j = 0; j < n; ++j)
                {
                    a[i][j] -= a[h][j] * factor;
                }
            }
        }
    }
    //    for (int i = 0; i < m; ++i)
    //{
    // for (int j = 0; j < n; ++j)
    //{
    // cout << a[i][j] << ' ';
    //}
    // cout << endl;
    //}
    // cout << "*********" << endl;
    for (int k = n - 2; k >= 0; k--)
    {
        for (int i = 0; i < m; ++i)
        {
            if (abs(a[i][k]) <= eps)
                continue;

            if (!done[k])
            {
                ans[k] = a[i][n - 1] / a[i][k];
                done[k] = true;
            }
            else
            {
                if (ans[k] != a[i][n - 1] / a[i][k])
                {
                    cout << "No solution!" << endl;
                    return 0;
                }
            }
        }
    }
    for (int i = 0; i < m; ++i)
        printf("%.3f ", ans[i]);
    return 0;
}