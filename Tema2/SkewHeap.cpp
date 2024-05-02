#include <fstream>
#include <iostream>
#include <chrono>
#include <algorithm>
using namespace std;

struct SkewHeap
{
    int key;
    SkewHeap* right;
    SkewHeap* left;

    SkewHeap()
    {
        key = 0;
        right = nullptr;
        left = nullptr;
    }

    static SkewHeap* mergee(SkewHeap* h1, SkewHeap* h2)
    {
        if (h1 == NULL)
            return h2;
        if (h2 == NULL)
            return h1;

        if (h1->key < h2->key)
            swap(h1, h2);

        swap(h1->left, h1->right);

        h1->left = mergee(h2, h1->left);

        return h1;
    }
    static SkewHeap* Add(SkewHeap* h, int val)
    {
        SkewHeap* temp;
        temp = new SkewHeap;
        temp->key = val;
        h = h->mergee(temp, h);
        return h;
    }
};
SkewHeap* insertNumbers(int val_min, int val_max, long long cnt, SkewHeap* H) {
    int randomNumber;
    srand(static_cast<unsigned int>(std::time(nullptr)));
    for (int i = 0; i < cnt; i++) {
        //if (i % 1000 == 0)
           // cout << i << endl;
        randomNumber = val_min + rand() % (val_max - val_min + 1);
        H = SkewHeap::Add(H, randomNumber);
    }
    return H;
}

SkewHeap* removeNumbers(long long cnt, SkewHeap* H) {
    int k = 0;
    for (int i = 0; i < cnt; i++) {
        k++;
        //cout << H->key << "\n";
        SkewHeap* del = H;
        H = SkewHeap::mergee(H->left, H->right);
        delete del;
    }
    cout << endl << k;
    return H;
}

void test(int val_min, int val_max, long long cnt, SkewHeap* H) {
    auto start = std::chrono::high_resolution_clock::now();
    H = insertNumbers(val_min, val_max, cnt, H);
    H = removeNumbers(cnt, H);
    auto end = std::chrono::steady_clock::now();
    std::chrono::duration<double> duration = end - start;
    cout << endl << duration.count();
}

int n, m, x, y, z;
ifstream f("date.in");
ofstream g("date.out");
int main()
{
    /*
    //multime de heapuri
    f>>n>>m;
    SkewHeap* heap[n+1];
    for (int i = 0; i <= n; ++i) {
        heap[i]=NULL;
    }
    int t=0;
    while(m--)
    {
        f>>x>>y;
        SkewHeap* temp;
        cout<<x<<'\n';
        if(x==1)
        {
            f>>z;
            heap[y]->Add(heap[y],z);
        }
        else
        if(x==2)
        {
            g<<heap[y]->key<<'\n';
            heap[y]=heap[y]->mergee(heap[y]->left,heap[y]->right);

        }
        else
        if(x==3)
        {
            f>>z;
            heap[y]=heap[y]->mergee(heap[y],heap[z]);
        }
    }*/
    SkewHeap* H = new SkewHeap();
    test(pow(10, 3), pow(10, 7), pow(10, 9), H);
    return 0;

}
