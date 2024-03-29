#include <iostream>
#include <vector>
#include <cstdlib>
#include <chrono>
#include <ctime>
#include <cmath>
#include <thread>
#include <stdexcept>
#include <fstream>
#include <algorithm>
#define f cin
#define g cout
long long cnt;
using namespace std;
vector<long long>get_number(long long val_min,long long val_max,long long cnt)
{
    vector<long long>a;
    srand(static_cast<unsigned int>(std::time(nullptr)));
    for(long long i=0; i<cnt; i++)
    {
        long long randomNumber = val_min + rand() % (val_max - val_min + 1);
        a.push_back(randomNumber);
    }
    return a;
}
vector<long long>test_insertion(long long val_min,long long val_max,long long cnt)
{

    vector<long long>a;
    const int n=10;
    srand(static_cast<unsigned int>(std::time(nullptr)));
    for(long long i=0; i<n; i++)
    {
        long long randomNumber = val_min + rand() % (val_max - val_min + 1);
        a.push_back(randomNumber);
    }
    for(long long i=n;i<=cnt;i++)
        a.push_back(i);
    return a;
}
vector<long long>a,cpy;
bool inTime = true;
struct TimeOutException : public std::exception {};
void MergeSort(long long st,long long dr)
{
    if (!inTime)
        throw TimeOutException{};
    if(st==dr)
        return;
    long long mj=st+(dr-st)/2;
    MergeSort(st,mj);
    MergeSort(mj+1,dr);
    long long nr_st=st,nr_dr=mj+1,cnt=0;
    long long* b = new long long[dr-st+1];
    while(nr_st<=mj&&nr_dr<=dr)
    {
        if(a[nr_st]<=a[nr_dr])
            b[cnt++]=a[nr_st++];
        else
            b[cnt++]=a[nr_dr++];
    }
    while(nr_st<=mj)
        b[cnt++]=a[nr_st++];
    while(nr_dr<=dr)
        b[cnt++]=a[nr_dr++];
    for(int i=0; i<=dr-st; i++)
        a[i+st]=b[i];
    delete b;
}
void InsertionSort()
{
    for(long long i=1; i<a.size(); i++)
    {
        long long val=a[i];
        long long last=i-1;
        while(a[last]>val&&last>=0){
            if (!inTime)
                throw TimeOutException{};
            a[last+1]=a[last],last--;
        }
        a[last+1]=val;
    }
}
void Insert_Heap(long long val, long long* h, long long &nr)
{
    h[++nr]=val;
    long long cnt=nr;
    while(h[cnt]<h[cnt/2]&&cnt>1)
    {
        swap(h[cnt],h[cnt/2]);
        cnt/=2;
    }
}
void Erase_Heap(long long* h, long long &nr)
{
    swap(h[1],h[nr]);
    nr--;
    long long nod=1;
    while(true)
    {
        long long st=nod*2,dr=nod*2+1,mn=-1;
        if(st<=nr)
            mn=st;
        else
            break;
        if(dr<=nr)
            if(h[st]>h[dr])
                mn=dr;
        if(h[mn]<h[nod])
        {
            swap(h[nod],h[mn]);
            swap(nod,mn);
        }
        else
            return;

    }
}
void HeapSort()
{
    long long* h = new long long[a.size()];
    long long nr = 0;
    while(!a.empty())
    {
        if (!inTime)
            throw TimeOutException{};
        long long val=a.back();
        a.pop_back();
        Insert_Heap(val, h, nr);
    }
    while(nr>0)
    {
        if (!inTime)
            throw TimeOutException{};
        a.push_back(h[1]);
        Erase_Heap(h, nr);
    }
}
void RadixSort_10()
{
    long long p=1,mx,n=a.size();
    vector<long long>nr(11),aux(n+1);
    for(int i=0; i<n; i++)
        mx=max(mx,a[i]);
    for(int poz=1; p<=mx; poz++)
    {
        for(int i=0; i<10; i++)
            nr[i]=0;
        for(long long i=0; i<n; i++)
            nr[(a[i]/p)%10]++;
        for(int i=1; i<=9; i++)
            nr[i]+=nr[i-1];
        for(long long i=n-1; i>=0; i--)
        {
            if (!inTime)
                throw TimeOutException{};
            aux[nr[a[i]/p%10]]=a[i];
            nr[a[i]/p%10]--;
        }
        for(long long i=0; i<n; i++)
            a[i]=aux[i+1];
        p=p*10;
    }
    std::vector<long long>().swap(nr);
    std::vector<long long>().swap(aux);
}
void RadixSort_16()
{
    long long p=1,mx,n=a.size();
    int n_16=(1<<16);

    vector<long long>nr(n_16),aux(a.size());
    for(int poz=1; poz<=2; poz++)
    {
        for(int i=0; i<n_16; i++)
            nr[i]=0;
        for(long long i=0; i<n; i++)
            nr[(a[i]/p)%n_16]++;
        for(int i=1; i<n_16; i++)
            nr[i]+=nr[i-1];
        for(long long i=n-1; i>=0; i--)
        {
            if (!inTime)
                throw TimeOutException{};
            aux[nr[a[i]/p%n_16]]=a[i];
            nr[a[i]/p%n_16]--;
        }
        for(long long i=0; i<n; i++)
            a[i]=aux[i+1];
        p*=n_16;
    }
    std::vector<long long>().swap(nr);
    std::vector<long long>().swap(aux);
}
void ShellSort()
{
    vector<long long> gaps;
    long long N = a.size();
    int l = int(log2(N));
    for(int i = 1; i <= l; i++)
        gaps.push_back(2 * static_cast<long long>(N / (pow(2, i) * 2)) + 1);
    for(long long x = 0; x < gaps.size(); x++)
    {
        long long gap = gaps[x];
        for(long long i = gap; i < N; i++)
        {
            if (!inTime)
                throw TimeOutException{};
            long long temp = a[i];
            long long j;
            for(j = i; (j >= gap) && (a[j - gap] > temp); j -= gap)
                a[j] = a[j - gap];
            a[j] = temp;
        }
    }
}

int divide(long long st,long long dr)
{
    long long mij = st + (dr - st) / 2;
    if (a[st] > a[mij]) {
        swap(a[st], a[mij]);
    }
    if (a[mij] > a[dr]) {
        swap(a[mij], a[dr]);
    }
    if (a[st] > a[mij]) {
        swap(a[st], a[mij]);
    }
    long long piv = a[mij];
    swap(a[mij], a[dr]);
    long long i=st-1;
    for(long long j=st; j<dr; j++)
        if(a[j]<piv)
        {
            i++;
            swap(a[i],a[j]);
        }
    swap(a[i+1],a[dr]);
    return i+1;
}
void QuickSort(long long st, long long dr)
{
    if (!inTime)
        throw TimeOutException{};
    if(st>=dr)
        return;
    int pivot=divide(st,dr);
    QuickSort(st,pivot-1);
    QuickSort(pivot+1,dr);
}

long long minVal, maxVal, elemCnt;
bool isActive;
double Time_sort(int tip)
{
    inTime = true;
    isActive = true;
    auto start = std::chrono::high_resolution_clock::now();
    std::thread([]{
        auto startTime = std::chrono::steady_clock::now();
        while (isActive)
        {
            auto endTime = std::chrono::steady_clock::now();
            std::chrono::duration<double> duration = endTime - startTime;
            if (duration.count() >= 300)
                break;
        }
        inTime = false;
        }
    ).detach();
    a=cpy;
    if(tip==1)
    {
        g<<"Timpul pentru MergeSort este: ";
        MergeSort(0,a.size()-1);
    }
    else if(tip==2)
    {
        g<<"Timpul pentru InsertionSort este: ";
        InsertionSort();
    }
    else if(tip==3)
    {
        g<<"Timpul pentru HeapSort este: ";
        HeapSort();
    }
    else if(tip==4)
    {
        if (minVal < 0 || maxVal < 0)
            throw 0;
        g<<"Timpul pentru RadixSortBaza10 este: ";
        RadixSort_10();
    }
    else if(tip==5)
    {
        if (minVal < 0 || maxVal < 0)
            throw 0;
        g<<"Timpul pentru RadixSortBaza2 este: ";
        RadixSort_16();
    }
    else if(tip==6)
    {
        g<<"Timpul pentru ShellSort este: ";
        ShellSort();
    }
    else if(tip==7)
    {
        g<<"Timpul pentru QuickSort este: ";
        QuickSort(0 ,a.size()-1);
    }
    else if(tip==8)
    {
        g<<"Timpul pentru IntroSort(default): ";
        sort(a.begin(), a.end());
    }
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> duration = end - start;
    isActive = false;
    return duration.count();
}
void afisare()
{
    for(auto x : a)
        cout<<x<<" ";
}
bool is_sorted()
{
    long long prev = a[0];
    long long i = 1;
    while (i < a.size() - 1)
    {
        if (a[i] < prev)
            return false;
        else prev = a[i];
        i++;
    }
    std::vector<long long>().swap(a);
    return true;
}

int main()
{
    cout<<"Sortari: \nMerge-1\nInsertion-2\nHeap-3\nRadixB10-4\nRadixB2-5\nShell-6\nQuick-7\nIntroSort-8\n";
    int S;
    cin>>S;
    bool aproapeSortat;
    cout<<"Aproape sortat?: ";
    cin>>aproapeSortat;
    cout<<endl;
    ifstream f("input.in");
    int inputs;
    f>>inputs;
    for(int i=0; i<inputs; i++)
    {
        try{
        f>>minVal>>maxVal>>elemCnt;
        g<<minVal<<" "<<maxVal<<" "<<elemCnt<<" -> ";
        if (aproapeSortat)
            cpy = test_insertion(minVal, maxVal, elemCnt);
        else
           cpy=get_number(minVal, maxVal, elemCnt);
        g<<Time_sort(S)<<endl;
        g<<"Este bine sortat: "<<is_sorted()<<endl;
        }
        catch(...){g<<"Nu poate sorta!\n";std::vector<long long>().swap(a);}
    }
    return 0;
}
