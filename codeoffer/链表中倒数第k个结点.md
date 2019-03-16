# 链表中倒数第k个结点

C++，链表。

如果走两趟，会超市，肯定也不符合题目的意思。

走一趟的办法：

1. 让一个指针跑到第k-1个结点；
2. 第二个指针指向第一个结点；
3. 两个指针一起走；
4. 如果前一个指针走到了最后一个结点，那么后一个指针的位置就是倒数第k个结点的位置。

**然后注意一些特殊情况，比如k很大，比如链表为空的情况。**

代码如下：

```cpp
/*
struct ListNode {
	int val;
	struct ListNode *next;
	ListNode(int x) :
			val(x), next(NULL) {
	}
};*/
class Solution {
public:
    ListNode* FindKthToTail(ListNode* pListHead, unsigned int k) {
        if (!pListHead || k == 0) return NULL;
        ListNode *p = pListHead;
        ListNode *q = p;
        for (int i = 0; i < k - 1; i++) {
            if (p->next) p = p->next;
            else return NULL;
        }
        while (p->next) {
            p = p->next;
            q = q->next;
        }
        return q;
    }
};
```

