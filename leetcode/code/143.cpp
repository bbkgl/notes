#include <cstdio>
#include <iostream>
#include <vector>

/**
 * Definition for singly-linked list.
*/
struct ListNode {
    int val;
    ListNode *next;
    ListNode(int x) : val(x), next(NULL) {}
};


class Solution {
public:
    void reorderList(ListNode* head) {
        if (!head)
            return ;
        ListNode *dummy = new ListNode(0);
        dummy->next = head;
        head = dummy;
        int cnt = 0;
        while (head->next) {
            head = head->next;
            cnt++;
        }
        solve(dummy->next, cnt, 1);
        head = dummy->next;
    }

    ListNode *solve(ListNode *root, const int &cnt, int depth) {
        if (cnt % 2 == 0 && depth == cnt / 2) {
            ListNode *next = root->next;
            ListNode *rn_node = next->next;
            next->next = nullptr;
            return rn_node;
        } else if (cnt % 2 == 1 && depth == cnt / 2 + 1) {
            ListNode *next = root->next;
            root->next = nullptr;
            return next;
        }
        ListNode *next = solve(root->next, cnt, depth + 1);
        ListNode *next_next = root->next;
        root->next = next;
        ListNode *rn_next = next->next;
        next->next = next_next;
        return rn_next;
    }
};

ListNode *get_list(const std::vector<int> &v) {
    ListNode *dummy = new ListNode(0);
    ListNode *head = dummy;
    for (const int &it : v) {
        head->next = new ListNode(it);
        head = head->next;
    }
    return dummy->next;
}

void print_list(const ListNode *root) {
    if (!root) return ;
    std::cout << root->val << "\t";
    print_list(root->next);
}

int main() {
    std::vector<int> v = {1, 2, 3, 4, 5};
    ListNode *root = get_list(v);
    std::cout << "BEFORE:" << std::endl;
    print_list(root);
    std::cout << "\nAFTER:" << std::endl;
    (new Solution())->reorderList(root);
    print_list(root);
    std::cout << std::endl;
    return 0;
}