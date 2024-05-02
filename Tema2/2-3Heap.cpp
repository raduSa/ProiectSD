#include <fstream>
#include <algorithm>
#include <vector>
#include <chrono>
#include <iostream>

using namespace std;
ifstream f("date.in");
ofstream g("date.out");
class Node {
public:
    int value;
    std::vector<Node*> children;
    Node* parent;
    Node(int val) : value(val), parent(nullptr) {}
};

class MaxHeap23 {
private:
    Node* root;

public:
    MaxHeap23() : root(nullptr) {}

    void insert(int val) {
        Node* newNode = new Node(val);
        if (root == nullptr) {
            root = newNode;
        }
        else {
            insert_(root, newNode);
        }
        bubbleUp(newNode);
    }

    void insert_(Node* current, Node* newNode)
    {
        while (current->children.size() >= 3)
            current = current->children.back();
        current->children.push_back(newNode);
        newNode->parent = current;
    }

    void bubbleUp(Node* node) {
        while (node != root && node->value > node->parent->value) {
            swap(node->value, node->parent->value);
            node = node->parent;
        }
    }

    bool IsEmpty()
    {
        return (root == nullptr);
    }

    Node* findLastNode(Node* node)
    {
        if (node == nullptr)
            return nullptr;
        Node* current = node;
        while (!current->children.empty())
            current = current->children.back();
        return current;
    }
    int extractMax() {
        if (root == nullptr) {
            throw std::runtime_error("Heap is empty!");
        }
        int max = root->value;

        removeMax();
        return max;
    }

    void removeMax() {
        /// std::cout<<root->value<<'\n';
        Node* last = findLastNode(root);
        if (last == root) {
            delete root;
            root = nullptr;
            return;
        }
        root->value = last->value;
        last->parent->children.pop_back();
        delete last;
        percolateDown(root);
    }

    void percolateDown(Node* node) {
        while (node) {
            Node* nxt = node;
            for (Node* child : node->children)
                if (child->value > nxt->value)
                    nxt = child;
            if (node->value < nxt->value)
            {
                std::swap(node->value, nxt->value);
                node = nxt;
            }
            else
                break;
        }
    }
};

void insertNumbers(int val_min, int val_max, int cnt, MaxHeap23* H) {
    int randomNumber;
    srand(static_cast<unsigned int>(std::time(nullptr)));
    for (int i = 0; i < cnt; i++) {
        if (i % 1000 == 0)
            cout << i << endl;
        randomNumber = val_min + rand() % (val_max - val_min + 1);
        H->insert(randomNumber);
    }
}

void removeNumbers(int cnt, MaxHeap23* H) {
    int k = 0;
    for (int i = 0; i < cnt; i++) {
        k++;
        //cout << H->key << "\n";
        H->extractMax();
    }
    cout << endl << k;
}

void test(int val_min, int val_max, int cnt, MaxHeap23* H) {
    auto start = std::chrono::high_resolution_clock::now();
    insertNumbers(val_min, val_max, cnt, H);
    removeNumbers(cnt, H);
    auto end = std::chrono::steady_clock::now();
    std::chrono::duration<double> duration = end - start;
    cout << endl << duration.count();
}

void insertAndDel(int val_min, int val_max, int cnt, MaxHeap23* H) {
    auto start = std::chrono::high_resolution_clock::now();
    int randomNumber;
    srand(static_cast<unsigned int>(std::time(nullptr)));
    for (int i = 0; i < cnt; i++) {
        if (i && i % 1000 == 0) {
            cout << i << " " << H->extractMax() << "\n";
        }
        randomNumber = val_min + rand() % (val_max - val_min + 1);
        H->insert(randomNumber);
    }
    auto end = std::chrono::steady_clock::now();
    std::chrono::duration<double> duration = end - start;
    cout << endl << duration.count();
}

int main() {
    /*int n,m,x,y,tip;
    f>>n>>m;

    MaxHeap23 heap[n+2];
    while(m--)
    {
        f>>tip;
        if(tip==1)
        {
            f>>x>>y;
            heap[x].insert(y);
        }
        else
        if(tip==2)
        {
            f>>x;
            g<<heap[x].extractMax()<<'\n';
        }
        else
        {
            f>>x>>y;
            while(!heap[y].IsEmpty())
            {
                heap[x].insert(heap[y].extractMax());
            }
        }
    }*/
    MaxHeap23* H = new MaxHeap23();
    //test(pow(10, 3), pow(10, 7), 3 * pow(10, 5), H);
    insertAndDel(pow(10, 3), pow(10, 7), 3 * pow(10, 5), H);
    return 0;
}
