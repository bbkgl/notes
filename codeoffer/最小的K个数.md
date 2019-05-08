# [最小的K个数](https://www.nowcoder.com/practice/6a296eb82cf844ca8539b57c23e6e9bf?tpId=13&tqId=11182&rp=2&ru=/ta/coding-interviews&qru=/ta/coding-interviews/question-ranking )

C++，topK问题。

一般来说这种题有两种种“可行”的方法：

- 最小/大堆，O(nlogk)；
- 快排的partition思想，O(n)；

在C++中，可以用STL中的优先队列来建立最小堆，但面试手撕代码的时候可不会让用库函数和容器。所以这里用最大堆来实解决topK的问题，注意三个测例的坑点：

- k <= 0，报超时或者段错误；
- k > input.size()，报超时或者段错误；
- input为空，段错误。

代码如下

```cpp
class Solution {
public:
    vector<int> GetLeastNumbers_Solution(vector<int> input, int k) {
        if (input.empty() || k == 0 || k > input.size())
            return vector<int> {};
        vector<int> heap(input.begin(), input.begin() + k);
        for (int i = k / 2 - 1; i >= 0; i--)
            adjust(heap, i);
        for (int i = k; i < input.size(); i++) {
            if (input[i] < heap[0]) {
                swap(input[i], heap[0]);
                adjust(heap, 0);
            }
        }
        return heap;
    }
    
    void adjust(vector<int> &heap, int pa) {
        while (pa < heap.size()) {
            int child = pa * 2 + 1;
            if (child >= heap.size()) 
                break;
            if (child + 1 < heap.size() && heap[child+1] > heap[child])
                child++;
            if (heap[pa] < heap[child]) 
                swap(heap[child], heap[pa]);
            else
                break;
            pa = child;
        }
    }
};
```