# [包含min函数的栈](https://www.nowcoder.com/practice/4c776177d2c04c2494f2555c9fcc1e49?tpId=13&tqId=11173&rp=1&ru=/ta/coding-interviews&qru=/ta/coding-interviews/question-ranking ) 

C++，顺序表实现，4ms。

```cpp
class Solution {
public:
    void push(int value) {
        if (stk.empty()) {
            min_value = value;
            min_i = 0;
        } else if (value < min_value) {
            min_value = value;
            min_i = stk.size();
        }
        stk.push_back(value);
    }
    void pop() {
        int t = stk.back();
        stk.pop_back();
        if (min_i >= stk.size() && !stk.empty()) {
            min_value = stk.front();
            min_i = 0;
            for (int i = 0; i < stk.size(); i++) {
                if (stk[i] < min_value) {
                    min_i = i;
                    min_value = stk[i];
                }
            }
        }
    }
    int top() {
        return stk.back();
    }
    int min() {
        return min_value;
    }
private:
    vector<int> stk;
    int min_i, min_value;
};
```

C++，辅助栈实现，6ms。

```cpp
class Solution {
public:
    void push(int value) {
        if (s2.empty() || value <= s2.top())
            s2.push(value);
        s1.push(value);
    }
    void pop() {
        if (s1.top() == s2.top())
            s2.pop();
        s1.pop();
    }
    int top() {
        return s1.top();
    }
    int min() {
        return s2.top();
    }
private:
    stack<int> s1, s2;
};
```

