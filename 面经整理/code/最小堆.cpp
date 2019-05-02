#include <cstdio>
#include <vector>
#include <iostream>
#include <cstdlib>
#include <ctime>
#include <memory.h>
#include <bits/unique_ptr.h>

const int K = 7;

using namespace std;

class MinHeap {
public:
    MinHeap();
    explicit MinHeap(int size);
    explicit MinHeap(vector<int> _heap);

    int Insert(int x);

    void Show() {
        for (int i = 0; i < size; i++)
            cout << heap[i] << "\t";
        cout << endl;
    }

private:

    void Create();

    void SiftDown(int);

    vector<int> heap;
    int size;
};

MinHeap::MinHeap() {
    size = 0;
};

MinHeap::MinHeap(int _size) {
    heap.resize(_size);
    size = _size;
    Create();
}

MinHeap::MinHeap(vector<int> _heap) {
    heap = _heap;
    size = _heap.size();
    Create();
}

void MinHeap::Create() {
    for (int i = size / 2 - 1; i >= 0; i--)
        SiftDown(i);
}

void MinHeap::SiftDown(int pa) {
    int child = pa * 2 + 1;
    while (child < size) {
        if (child + 1 < size && heap[child + 1] < heap[child])
            child++;
        if (heap[pa] > heap[child])
            swap(heap[pa], heap[child]);
        else break;
        pa = child;
        child = pa * 2 + 1;
    }
}

int MinHeap::Insert(int x) {
    if (x > heap[0]) {
        heap[0] = x;
        SiftDown(0);
        return 0;
    }
    return 1;
}

int main() {
    vector<int> heap;
    srand(time(0));
    for (int i = 0; i < K; i++)
        heap.push_back(rand() % 200);
    unique_ptr<MinHeap> mh(new MinHeap(heap));
    mh->Show();
    return 0;
}