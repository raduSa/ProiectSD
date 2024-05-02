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

    bool isActiveRoot() const { return (parent && parent->isPassive() && isActive()); }

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
            //cout << key << " ";
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
    deque<Node*> head;

    //~Heap() { delete active; }

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
        // ultimele 3 noduri trebuie sa fie passive linkable
        if (!temp[0]->isPassiveLinkable() || !temp[1]->isPassiveLinkable() || !temp[2]->isPassiveLinkable())
            return false;

        //cout << "Root Degree Reduction\n";
        // sortez ult 3 noduri crescator dupa cheie
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
        map<int, deque<Node*>> map;
        bool stop = false;

        root->updateActiveRoot(map);

        for (const auto& pair : map)
            if (pair.second.size() >= 2 && !stop) {
                Node* x = pair.second[0];
                Node* y = pair.second[1];
                //cout << "Active Root Reduction on " << x->key << " " << y->key << " \n";
                if (x->key > y->key) 
                    swap(x, y);
                // sterge pe y din lista de copii
                auto it = find(y->parent->children.begin(), y->parent->children.end(), y);
                if (it != y->parent->children.end())
                    y->parent->children.erase(it);

                y->parent = x;
                x->children.push_front(y);
                x->rank++;
                x->loss = 0;
                y->loss = 0;
                Node* z = x->children[x->children.size() - 1];
                if (z->isPassiveLinkable()) {
                    root->children.push_back(z);
                    z->parent = root;
                    x->children.pop_back();
                }
                else if (z->isPassive()) {
                    int index = root->firstPassiveChildIndex();
                    auto it = root->children.begin() + index;
                    root->children.insert(it, z);
                    z->parent = root;
                    x->children.pop_back();
                }
                stop = true;
            }
        return stop;
    }

    bool oneNodeLossReduction() {
        Node* x = root->getActiveLossTwo();
        if (!x)
            return false;
        //cout << "One node Loss Reduction\n";
        Node* y = x->parent;

        root->children.push_front(x);
        x->parent = root;

        // sterge pe x din copii lui y
        auto it = find(y->children.begin(), y->children.end(), x);
        if (it != y->children.end())
            y->children.erase(it);

        x->loss = 0;
        if (y->isActive()) {
            y->rank--;
            if (!y->isActiveRoot())
                y->loss++;
        }
        return true;
    }

    bool twoNodeLossReduction() {
        map<int, deque<Node*>> map;
        bool stop = false;

        root->updateActiveNode(map);

        for (const auto& pair : map) {
            Node* x = nullptr; Node* y = nullptr;
            // each node in value
            for (const auto& c : pair.second)
                if (c->loss == 1 && c->isActive() && !c->isActiveRoot()) {
                    if (x)
                        y = c;
                    else x = c;
                }

            if (x && y && !stop) {
                stop = true;
                if (x->key > y->key)
                    swap(x, y);
                //cout << "Two node Loss Reduction\n";

                Node* z = y->parent;
                x->children.push_front(y);
                y->parent = x;
                // sterge pe y din copii lui z
                auto it = find(z->children.begin(), z->children.end(), y);
                if (it != z->children.end())
                    z->children.erase(it);

                x->rank++;
                x->loss = 0;
                y->loss = 0;

                if (z->isActive()) {
                    z->rank--;
                    if (!z->isActiveRoot())
                        z->loss++;
                }
            }
        }
        return stop;
    }
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

    smallSizeHeap->head.push_back(largeRootHeap->root);
    for (Node* node : largeSizeHeap->head)
        smallSizeHeap->head.push_back(node);
    smallRootHeap->head = smallSizeHeap->head;

    delete largeRootHeap;

    // restore invariants
    int numA = 1;
    int numR = 1;

    while (true) {
        bool stop = true;
        if (numA)
            if (smallRootHeap->activeRootReduction()) {
                numA--;
                stop = false;
            }
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

Heap* deleteMin(Heap* h) {
    if (!h->root)
        return h;
    int Min = 999999;
    Node* minNode = nullptr;
    for (const auto& child : h->root->children)
        if (child->key < Min) {
            Min = child->key;
            minNode = child;
        }
    if (!minNode) {
        h = new Heap();
        return h;
    }
    // leg nodurile de noul minim
    for (const auto& child : h->root->children) {
        if (child != minNode)
            if (child->isActive())
                minNode->children.push_front(child);
            else if (child->isPassiveLinkable())
                minNode->children.push_back(child);
            else {
                int index = minNode->firstPassiveChildIndex();
                auto it = minNode->children.begin() + index;
                minNode->children.insert(it, child);
            }
        child->parent = minNode;
    }
    delete h->root;
    h->root = minNode;
    // sterg nodul scos din coada
    auto it = find(h->head.begin(), h->head.end(), minNode);
    if (it != h->head.end())
        h->head.erase(it);
    h->size--;
    h->root->active = nullptr;// might leak
    
    for (int j = 0; j < min(2, int(h->head.size())); j++) {
        Node* c = h->head[0];
        h->head.pop_front();
        h->head.push_back(c);
        int ct = 2;
        while (ct--) {
            int k = c->children.size() - 1;
            if (k < 0)
                continue;
            if (c->children[k]->isPassiveLinkable()) {
                minNode->children.push_back(c->children[k]);
                c->children[k]->parent = minNode;
                c->children.pop_back();
                if (!c->isActiveRoot() && c->isActive())
                    c->loss++;
            }
            else if (c->children[k]->isPassive()) {
                int index = minNode->firstPassiveChildIndex();
                auto it = minNode->children.begin() + index;
                minNode->children.insert(it, c->children[k]);
                c->children[k]->parent = minNode;
                c->children.pop_back();
                if (!c->isActiveRoot() && c->isActive())
                    c->loss++;
            }
        }
    }
    if (!h->twoNodeLossReduction()) {
        h->oneNodeLossReduction();
    }
    while (h->activeRootReduction() || h->rootDegreeReduction()) {}
    return h;
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
    cout << "Q: ";
    for (const auto& node : h->head)
        cout << node->key << " ";
    cout << endl;
}

Heap* insertNumbers(int val_min, int val_max, int cnt, Heap* H)
{
    int randomNumber;
    srand(static_cast<unsigned int>(std::time(nullptr)));
    for (int i = 0; i < cnt; i++) {
        if (i % 1000 == 0)
            cout << i << endl;
        randomNumber = val_min + rand() % (val_max - val_min + 1);
        H = insert(H, randomNumber);
    }
    return H;
}

Heap* removeNumbers(int cnt, Heap* H) {
    int k = 0;
    for (int i = 0; i < cnt; i++) {
        k++;
        cout << H->root->key << "\n";
        H = deleteMin(H);
    }
    cout << k;
    return H;
}

int main()
{
    Heap* H = new Heap();
    /*H = insert(H, 1);
    H = insert(H, 9);
    H = insert(H, 8);
    H = insert(H, 3);
    H = insert(H, 4);
    H = insert(H, 2);
    H = insert(H, 5);
    H = insert(H, 6);
    H = insert(H, 0);
    H = insert(H, 7);

    Heap* H2 = new Heap();
    H2 = insert(H2, 10);
    H2 = insert(H2, 5);
    H2 = insert(H2, 7);
    //bfs(H);
    H = merge(H, H2);
    bfs(H);
    cout << "Min is: " << H->root->key << endl;
    H = deleteMin(H);
    bfs(H);

    cout << endl << H->root->getHeight();
    map<int, deque<Node*>> myMap;
    H->root->updateActiveRoot(myMap);

    // print map<int, deque<Node*>>
    for (const auto& pair : myMap) {
        std::cout << "Key: " << pair.first << ", Values: ";
        for (Node* value : pair.second) {
            std::cout << value->key << " ";
        }
        std::cout << std::endl;
    }*/
    H = insertNumbers(1, 1000, pow(10, 5), H);
    cout << "delete";
    H = removeNumbers(1000, H);
    return 0;
}
