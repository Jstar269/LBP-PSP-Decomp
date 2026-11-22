/* LBPMATCH module=EBOOT.BIN start=0x0888018c end=0x08880270 symbol=FUN_0888018c lang=c++ */
/* Hand-written reconstruction from the disassembly (lane oc-lbp-20261121, Antigravity); exact match verified; names and types await human review. */

struct Heap {
    unsigned char pad[32];
    int lock;
};

inline Heap *&sys_heap()
{
    static Heap *heap;
    return heap;
}

extern "C" void FUN_08ad3088(void *);
extern "C" void FUN_08ad2810(void *);
extern "C" void FUN_08adbc24(Heap *, void *, int);

struct ListNode {
    ListNode *next;
    ListNode *prev;
};

struct TreeNode {
    int color;
    TreeNode *parent;
    TreeNode *left;
    TreeNode *right;
    int key;
    ListNode list;
};

extern "C" void FUN_0888018c(void *self, TreeNode *x)
{
    while (x != 0) {
        FUN_0888018c(self, x->right);
        TreeNode *y = x->left;
        ListNode *end = &x->list;
        ListNode *cur = x->list.next;
        while (cur != end) {
            ListNode *next = cur->next;
            Heap *h = sys_heap();
            FUN_08ad3088(&h->lock);
            FUN_08adbc24(h, cur, 36);
            FUN_08ad2810(&h->lock);
            cur = next;
        }
        Heap *h = sys_heap();
        FUN_08ad3088(&h->lock);
        FUN_08adbc24(h, x, 28);
        FUN_08ad2810(&h->lock);
        x = y;
    }
}
