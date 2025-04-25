#include <iostream>
#include <vector>
using namespace std;

class Solution
{
public:
    int kthSmallestElement(vector<int> &arr, int k)
    {
        int l = 0, r = arr.size() - 1;
        return ksel(arr, k, l, r);
    }

private:
    int ksel(vector<int> &a, int k, int l, int r)
    {
        int i = l - 1, j = r + 1;
        int mid = a[l + r >> 1];
        while (i < j)
        {
            do
            {
                i++;
            } while (a[i] < mid);
            do
            {
                j--;
            } while (a[j] > mid);
            if (i < j)
            {
                int c = a[j];
                a[j] = a[i];
                a[i] = c;
            }
        }
        int fhalf = j - l + 1;
        if (fhalf == k)
            return a[j];
        if (fhalf > k)
            return ksel(a, k, l, j);
        return ksel(a, k - fhalf, j + 1, r);
    }
};

int main()
{
    // Input processing
    vector<int> a;
    Solution sol;
    int x;
    while (cin >> x)
    {
        a.push_back(x);
    }
    int k = a[a.size() - 1];
    a.pop_back();
    int kth = sol.kthSmallestElement(a, k);
    // Output the result
    cout << kth << endl;
    return 0;
}