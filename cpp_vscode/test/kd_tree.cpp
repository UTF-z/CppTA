#include <iostream>
using namespace std;
struct Item
{
    int key[2];
    int value;
};

class Node
{
public:
    Node *left, *right;
    Item item;

    Node(Item _item) : item(_item), left(NULL), right(NULL)
    {
    }
};
// void display(Node *root)
//{
// if (root == NULL)
// return;
// cout << root->item.key[0] << ' ' << root->item.key[1] << endl;
// display(root->left);
// display(root->right);
//}
void insert(Node *&root, Item item, int dim)
{
    if (root == NULL)
    {
        root = new Node(item);
        return;
    }
    if (item.key[dim] <= root->item.key[dim])
    {
        insert(root->left, item, 1 - dim);
    }
    else
    {
        insert(root->right, item, 1 - dim);
    }
}

// If the queried key doesn't exist, return NULL.
Node *search(Node *root, int key[2], int dim)
{
    if (root == NULL)
        return NULL;
    if (root->item.key[dim] == key[dim] && root->item.key[1 - dim] == key[1 - dim])
    {
        return root;
    }
    if (key[dim] < root->item.key[dim])
    {
        return search(root->left, key, 1 - dim);
    }
    else
    {
        return search(root->right, key, 1 - dim);
    }
}

// Find the minimum value in a dimension(dimCmp)
Node *_findMin(Node *root, int dimCmp, int dim, int &currdim)
{
    if (root == NULL)
    {
        currdim = dim;
        return root;
    }
    if (dim == dimCmp)
    {
        if (root->left == NULL)
        {
            currdim = dim;
            return root;
        }
        return _findMin(root->left, dimCmp, 1 - dim, currdim);
    }
    int diml, dimr;
    Node *lm = _findMin(root->left, dimCmp, 1 - dim, diml);
    Node *rm = _findMin(root->right, dimCmp, 1 - dim, dimr);
    Node *ans = root;
    currdim = dim;
    if (lm)
    {
        if (lm->item.key[dimCmp] < ans->item.key[dimCmp])
        {
            ans = lm;
            currdim = diml;
        }
    }
    if (rm)
    {
        if (rm->item.key[dimCmp] < ans->item.key[dimCmp])
        {
            ans = rm;
            currdim = dimr;
        }
    }
    return ans;
}

Node *_findMax(Node *root, int dimCmp, int dim, int &currdim)
{
    if (root == NULL)
    {
        currdim = dim;
        return root;
    }
    if (dim == dimCmp)
    {
        if (root->right == NULL)
        {
            currdim = dim;
            return root;
        }
        return _findMax(root->right, dimCmp, 1 - dim, currdim);
    }
    int diml, dimr;
    Node *lm = _findMax(root->left, dimCmp, 1 - dim, diml);
    Node *rm = _findMax(root->right, dimCmp, 1 - dim, dimr);
    Node *ans = root;
    currdim = dim;
    if (lm)
    {
        if (lm->item.key[dimCmp] > ans->item.key[dimCmp])
        {
            ans = lm;
            currdim = diml;
        }
    }
    if (rm)
    {
        if (rm->item.key[dimCmp] > ans->item.key[dimCmp])
        {
            ans = rm;
            currdim = dimr;
        }
    }
    return ans;
}

Node *findMin(Node *root, int dimCmp, int dim)
{
    int currdim;
    return _findMin(root, dimCmp, dim, currdim);
}

void remove(Node *&root, int key[2], int dim)
{
    // cout << "******" << endl;
    // display(root);
    // cout << "******" << endl;
    if (root == NULL)
        return;
    if (root->item.key[0] == key[0] && root->item.key[1] == key[1])
    {
        int dimr, diml;
        Node *pr, *pl;
        pr = pl = NULL;
        if (root->right)
        {
            pr = _findMin(root->right, dim, 1 - dim, dimr);
        }
        else if (root->left)
        {
            pl = _findMax(root->left, dim, 1 - dim, diml);
        }
        if (pr)
        {
            root->item.key[0] = pr->item.key[0];
            root->item.key[1] = pr->item.key[1];
            root->item.value = pr->item.value;
            remove(root->right, pr->item.key, 1 - dim);
        }
        else if (pl)
        {
            root->item.key[0] = pl->item.key[0];
            root->item.key[1] = pl->item.key[1];
            root->item.value = pl->item.value;
            remove(root->left, pl->item.key, 1 - dim);
        }
        else
        {
            delete root;
            root = NULL;
        }
    }
    else
    {
        if (key[dim] <= root->item.key[dim])
        {
            remove(root->left, key, 1 - dim);
        }
        else
        {
            remove(root->right, key, 1 - dim);
        }
    }
}

// searchRange contains 4 values [x_min, x_max, y_min, y_max]
// Our test program will call this function with dim=0 treeRange={-9999, 9999, -9999, 9999}
int rangeSearch(Node *root, int dim, int searchRange[4], int treeRange[4])
{
    // TODO
    if (root == NULL)
    {
        return 0;
    }
    if (treeRange[0] > searchRange[1] || treeRange[1] < searchRange[0] || treeRange[2] > searchRange[3] || treeRange[3] < searchRange[2])
    {
        return 0;
    }
    int leftCount, rightCount;
    int temp = treeRange[dim * 2 + 1];
    treeRange[dim * 2 + 1] = root->item.key[dim];
    leftCount = rangeSearch(root->left, 1 - dim, searchRange, treeRange);
    treeRange[dim * 2 + 1] = temp;
    temp = treeRange[dim * 2];
    treeRange[dim * 2] = root->item.key[dim] + 1;
    rightCount = rangeSearch(root->right, 1 - dim, searchRange, treeRange);
    treeRange[dim * 2] = temp;
    if (root->item.key[dim] >= searchRange[2 * dim] && root->item.key[dim] <= searchRange[2 * dim + 1] && root->item.key[1 - dim] >= searchRange[2 - 2 * dim] && root->item.key[1 - dim] <= searchRange[3 - 2 * dim])
        return leftCount + rightCount + 1;
    return leftCount + rightCount;
}

int ts[4] = {-9999, 9999, -9999, 9999};
int key[2];
int rs[4];
int main()
{
    int n;
    cin >> n;
    char op[20];
    Node *root = NULL;
    while (n--)
    {
        scanf("%s", op);
        if (op[3] == 'e')
        {
            int x, y, v;
            cin >> x >> y >> v;
            Item i;
            i.key[0] = x;
            i.key[1] = y;
            i.value = v;
            insert(root, i, 0);
        }
        else if (op[3] == 'g')
        {
            int x0, x1, y0, y1;
            cin >> x0 >> x1 >> y0 >> y1;
            rs[0] = x0;
            rs[1] = x1;
            rs[2] = y0;
            rs[3] = y1;
            int count = rangeSearch(root, 0, rs, ts);
            cout << count << endl;
        }
        else if (op[3] == 'o')
        {
            int x, y;
            cin >> x >> y;
            key[0] = x;
            key[1] = y;
            remove(root, key, 0);
        }
        else if (op[3] == 'r')
        {
            int x, y;
            cin >> x >> y;
            key[0] = x;
            key[1] = y;
            Node *ptr = search(root, key, 0);
            if (!ptr)
                cout << "NULL" << endl;
            else
                cout << ptr->item.value << endl;
        }
        else if (op[3] == 'd')
        {
            int dim;
            cin >> dim;
            Node *ptr = findMin(root, dim, 0);
            if (!ptr)
                cout << "NULL" << endl;
            else
                cout << ptr->item.value << endl;
        }
    }
    return 0;
}