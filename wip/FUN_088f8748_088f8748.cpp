/* LBPMATCH module=EBOOT.BIN start=0x088f8748 end=0x088f87ec symbol=FUN_088f8748 lang=c++ */
/* Hand-written reconstruction from the disassembly (lane oc-lbp-20261121, Antigravity); near match; names and types await human review. */

struct Node {
    Node *next;
    Node *prev;
    void *val;
};

struct ListHead {
    Node *next;
    Node *prev;
};

struct Obj {
    ListHead list1;
    ListHead list2;
    int pad[8];
    int f30;
};

extern "C" void *DAT_08be84a0;
extern "C" void *DAT_08beee0c;
extern "C" void FUN_08adbc24(void *, Node *, int);
extern "C" int FUN_088f21ec(void *, void *);

extern "C" bool FUN_088f8748(Obj *param_1)
{
    Node *n = param_1->list1.next;
    while (n != (Node *)&param_1->list1) {
        Node *next = n->next;
        FUN_08adbc24(DAT_08be84a0, n, 12);
        n = next;
    }
    param_1->list1.next = (Node *)&param_1->list1;
    param_1->list1.prev = (Node *)&param_1->list1;
    param_1->f30 = 0;
    for (Node *m = param_1->list2.next; m != (Node *)&param_1->list2; m = m->next) {
        if (FUN_088f21ec(DAT_08beee0c, m->val) != 0) {
            param_1->f30++;
        }
    }
    return param_1->list2.next != (Node *)&param_1->list2;
}

// Near-miss note: List2 sentinel hoisted in s2 vs recomputed addiu v0,s1,8.
// near miss from lane oc-lbp-20261121 (unreviewed)
