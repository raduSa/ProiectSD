#include <iostream>
#include <vector>
#include <deque>
#include <algorithm>
#include <map>

using namespace std;

struct Active {
    bool flag = true;
};

class Node {
public:
    int key;
    Node* parent = nullptr;
    deque<Node*> children;
    Active* active = nullptr;
    int loss = 0;
    int rank = 0;
    bool color = false; // idk

    Node(const int& key) : key(key) {}

    int getHeight() {
        if (children.size() == 0)
            return 1;
        int maxHeight = 0;
        for (auto child : children)
            maxHeight = max(maxHeight, child->getHeight());
        return 1 + maxHeight;
    }

    int getWidth() {
        if (children.size() == 0)
            return 1;
        int maxWidth = 0;
        for (auto child : children)
            maxWidth += child->getWidth();
        return maxWidth;
    }

    int getRank() const {
        if (isActive())
            return rank;
        return 0;
    }

    int getLoss() const {
        if (isActive())
            return loss;
        return 0;
    }

    bool isPassiveLinkable() {
        if (isActive())
            return false;

        for (auto child : children)
            if (!child->isPassiveLinkable())
                return false;

        return true;
    }

    bool isActive() const { return active && active->flag; }
        
    bool isPassive() const { return !isActive(); }

    bool isActiveRoot() const { return parent && parent->isPassive() && isActive(); }

    int firstPassiveChildIndex() {
        int i;
        for (i = 0; i < children.size(); i++)
            if (children[i]->isPassive())
                return i;
        return i;
    }

    // creeaza un map cu key un rang iar valori nodurile cu rangul respectiv
    // se face pe un active root
    void updateActiveRoot(map<int, deque<Node*>>& map) {
        if (isActiveRoot()) {
            if (!map.count(rank))
                map[rank] = deque<Node*>();
            map[rank].push_back(this);
        }
        for (auto child : children)
            child->updateActiveRoot(map);
    }

    // la fel dar nu e root, doar active node
    void updateActiveNode(map<int, deque<Node*>>& map) {
        if (isActive()) {
            if (!map.count(rank))
                map[rank] = deque<Node*>();
            map[rank].push_back(this);
        }
        for (auto child : children)
            child->updateActiveNode(map);
    }

    // idk yet
    void updateActivePosiveNode(map<int, deque<Node*>>& map) {
        if (isActive() && !isActiveRoot() && loss > 0) {
            if (!map.count(rank))
                map[rank] = deque<Node*>();
            map[rank].push_back(this);
        }
        for (auto child : children)
            child->updateActivePosiveNode(map);
    }

    Node* getActiveLossTwo() {
        if (isActive() && !isActiveRoot() && loss == 2)
            return this;
        for (auto child : children)
            if (child->getActiveLossTwo())
                return child;
        return nullptr;
    }

    Node* findNode(const Node* node) {
        if (node == this)
            return this;
        for (auto child : children)
            if (child->findNode(node))
                return child;
        return nullptr;
    }
};

struct Heap {
    Node* root = nullptr;
    int size = 0;
    Active* active = new Active();

    Node* findNode(const Node* node) {
        if (!root)
            return nullptr;
        return root->findNode(node);
    }

    bool rootDegreeReduction() {
        int n = root->children.size();
        if (n < 3)
            return false;
        
        vector<Node*> temp = { root->children[n - 1], root->children[n - 2], root->children[n - 3] };
        sort(temp.begin(), temp.end(), [](const Node* a, const Node* b) {
            return a->key < b->key;
            });

        root->children.pop_back();
        root->children.pop_back();
        root->children.pop_back();

        auto x = temp[0];
        auto y = temp[1];
        auto z = temp[2];

        x->active = active;
        y->active = active;
        y->children.push_back(z);
        z->parent = y;
        x->children.push_front(y);
        y->parent = x;
        root->children.push_front(x);

        x->loss = 0;
        y->loss = 0;
        z->loss = 0;

        x->rank = 1;
        y->rank = 0;
        z->rank = 0;

        return true;
    }

    bool activeRootReduction() {
        bool stop = false;

    }
    bool oneNodeLossReduction() {}
    bool twoNodeLossReduction() {}

    
};

Heap* merge(Heap* x, Heap* y) {
    // cazuri de baza
    if (x == y)
        return x;
    if (!x->root)
        return y;
    if (!y->root)
        return x;
    // determin heapul mai mic
    Heap* smallSizeHeap; Heap* largeSizeHeap;
    if (x->size > y->size) { smallSizeHeap = y; largeSizeHeap = x; }
    else { smallSizeHeap = x; largeSizeHeap = y; }

    smallSizeHeap->active->flag = false;
    Heap* smallRootHeap = x; Heap* largeRootHeap = y;
    if (x->root->key > y->root->key) {
        smallRootHeap = y; largeRootHeap = x;
    }
    if (smallRootHeap == smallSizeHeap)
        smallRootHeap->root->children.push_front(largeRootHeap->root);
    else
        smallRootHeap->root->children.push_back(largeRootHeap->root);

    largeRootHeap->root->parent = smallRootHeap->root;
    smallRootHeap->size += largeRootHeap->size;
    smallRootHeap->active = largeSizeHeap->active;

    // restore invariants
    int numA = 1;
    int numR = 1;

    while (true) {
        bool stop = true;
        if (numR)
            if (smallRootHeap->rootDegreeReduction()) {
                numR--;
                stop = false;
            }
        if (stop)
            break;
    }

    return smallRootHeap;
}

Heap* insert (Heap* h, const int& val) {
    Node* node = new Node(val);
    if (!h->root) {
        h->root = node;
        h->size = 1;
        h->active = new Active();
        return h;
    }
    Heap* newHeap = new Heap();
    newHeap->root = node;
    newHeap->size = 1;
    newHeap->active = new Active();
    return merge(h, newHeap);
}

void bfs(const Heap* h) {
    deque<Node*> q = { h->root };
    while (!q.empty()) {
        int n = q.size();
        for (int i = 0; i < n; i++) {
            Node* node = q.front();
            q.pop_front();
            std::cout << node->key << "-" << node->isActive() << " ";
            for (Node* child : node->children)
                q.push_back(child);
        }
        cout << endl;
    }
}

int main()
{
    Heap* H = new Heap();
    H = insert(H, 10);
    H = insert(H, 11);
    H = insert(H, 12);
    H = insert(H, 13);
    bfs(H);
    cout << endl << H->root->getHeight();
    map<int, deque<Node*>> myMap;
    H->root->updateActiveNode(myMap);

    // print map<int, deque<Node*>>
    for (const auto& pair : myMap) {
        std::cout << "Key: " << pair.first << ", Values: ";
        for (Node* value : pair.second) {
            std::cout << value->key << " ";
        }
        std::cout << std::endl;
    }
    return 0;
}
