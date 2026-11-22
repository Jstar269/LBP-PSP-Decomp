/* LBPMATCH module=EBOOT.BIN start=0x088f8960 end=0x088f8a44 symbol=FUN_088f8960 lang=c++ */
/* Hand-written reconstruction from the disassembly (lane oc-lbp-20261121, Antigravity); exact match verified; names and types await human review. */

struct Node {
    Node *next;
    Node *prev;
    void *val;
};

extern "C" void *DAT_08be84a0;
extern "C" void *DAT_08beee0c;
extern "C" int FUN_088f7fc8(void *, void *);
extern "C" int FUN_088f24a8(void *, void *, int *, void *);
extern "C" void *FUN_08adbb84(void *, unsigned int);
extern "C" void FUN_08adbc24(void *, void *, unsigned int);
extern "C" void FUN_08b88fac(Node *, Node *);
extern "C" void FUN_08b88fc8(Node *);

inline void *operator new(unsigned int, void *p) throw() { return p; }

struct ListHead {
    Node *next;
    Node *prev;
};

struct List {
    ListHead head;

    void pop_front() {
        Node *n = head.next;
        FUN_08b88fc8(n);
        FUN_08adbc24(DAT_08be84a0, n, sizeof(Node));
    }

    void push_back(void *val) {
        Node *n = (Node *)FUN_08adbb84(DAT_08be84a0, sizeof(Node));
        new (&n->val) void *(val);
        FUN_08b88fac(n, (Node *)&head);
    }
};

struct Obj {
    int pad0[4];
    List list1;
    List list2;
    int pad1[4];
    int f30;
};

extern "C" void FUN_088f8960(Obj *param_1)
{
    int local = 0;
    while (param_1->list1.head.next != (Node *)&param_1->list1.head) {
        void *item = param_1->list1.head.next->val;
        if (FUN_088f7fc8(param_1, item)) {
            param_1->list1.pop_front();
        } else {
            if (!FUN_088f24a8(DAT_08beee0c, item, &local, &param_1->f30)) {
                return;
            }
            param_1->list2.push_back(item);
            param_1->list1.pop_front();
        }
    }
}
