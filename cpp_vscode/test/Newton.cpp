#include <vector>
#include <iostream>
#include <cmath>
using namespace std;

/*
This is the class you need to implement. Do NOT output anything in your code. We will check your solution by
    Newton solution(n, a);
    double result = solution.solve();

Feel free to add any members in the class if needed.
*/

class Newton
{
    vector<double> a;
    vector<double> d;
    int n;

public:
    Newton(int _n, vector<double> _a) : n(_n), a(_a)
    {
        d = vector<double>(a.size());
        for (int i = 0; i < n; i++)
        {
            d[i] = a[i + 1] * (i + 1);
        }
    }

    double solve()
    {
        double x0 = 0, x1 = 0;
        double df = 0.0;
        double f = 0.0;
        int step = 0;
        do
        {
            x0 = x1;
            f = df = 0;
            for (int i = 0; i <= n; ++i)
            {
                f += a[i] * pow(x0, i);
            }
            for (int i = 0; i < n; i++)
            {
                df += d[i] * pow(x0, i);
            }
            x1 = x0 - (f / df);
            step++;
        } while (step < 10000 && abs(x1 - x0) > 1e-6);
        return x1;
    }
};

int main()
{
    vector<double> coeff;
    int n;
    cin >> n;
    for (int i = 0; i < n; ++i)
    {
        double e;
        cin >> e;
        coeff.push_back(e);
    }
    Newton solver(n - 1, coeff);
    cout << solver.solve() << endl;
    return 0;
}