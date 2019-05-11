# [和为S的两个数字](https://www.nowcoder.com/practice/390da4f7a00f44bea7c2f3d19491311b?tpId=13&tqId=11195&rp=3&ru=/ta/coding-interviews&qru=/ta/coding-interviews/question-ranking )

C++，双指针。

一个指针left指向数组头，一个right指向数组尾部。按理说我们应该将每个数都和其他n-1个数组合一下求和，但是题中给的其实是有序序列。这样就能利用有序序列的特性：

- 左边指针往中间移动和会越来越大；
- 右边指针往中间一定和会越来越小；
- 两个数距离越远，乘积越小，反之越大。

所有就使用双指针往中间移动就行了。

```cpp
class Solution {
public:
    vector<int> FindNumbersWithSum(vector<int> array,int sum) {
        int left = 0, right = array.size() - 1;
        vector<int> ans;
        while (ans.empty() && left < right) {
            int tsum = array[left] + array[right];
            if (tsum < sum)
                left++;
            else if (tsum > sum)
                right--;
            else {
                ans.push_back(array[left]);
                ans.push_back(array[right]);
            }
        }
        return ans;
    }
};
```

