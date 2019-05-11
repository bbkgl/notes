# [和为S的连续正数序列](https://www.nowcoder.com/practice/c451a3fd84b64cb19485dad758a55ebe?tpId=13&tqId=11194&tPage=3&rp=3&ru=/ta/coding-interviews&qru=/ta/coding-interviews/question-ranking )

C++，双指针。

用指针left、right维护一个窗口，将窗口内所有数之和与sum对比，然后根据结果调整窗口大小。如果大，left++，如果小，right++。

等差数列求和。。。

```cpp
class Solution {
public:
    vector<vector<int>> FindContinuousSequence(int sum) {
        vector<vector<int>> ans;
        int left = 1, right = 2;
        while (left < right) {
            int tsum = (left + right) * (right - left + 1) / 2;
            if (tsum == sum) {
                vector<int> v;
                for (int i = left; i <= right; i++) v.push_back(i);
                ans.push_back(v);
                left++;
            } else if (tsum < sum)
                right++;
            else
                left++;
        }
        return ans;
    }
};
```

