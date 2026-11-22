/* LBPMATCH module=EBOOT.BIN start=0x088f801c end=0x088f80e4 symbol=FUN_088f801c lang=c++ */
/* Hand-written reconstruction from the disassembly (lane oc-lbp-20261121, Antigravity); near match; names and types await human review. */

struct Item {
    unsigned int flags;
};

struct Node {
    Node *next;
    Node *prev;
    Item *val;
};

struct ListHead {
    Node *next;
    Node *prev;
};

struct Obj {
    int pad[2];
    ListHead list;
    int pad2[4];
    void *f20;
    void *f24;
    void *f28;
    int f2c;
};

extern "C" void *DAT_08beee0c;
extern "C" void *FUN_08a9e824(unsigned int);
extern "C" void FUN_088d42c4(Item *, void *, void *, void *, int *, int);
extern "C" void FUN_088f4ad0(void *, void *, int);

extern "C" void FUN_088f801c(Obj *param_1)
{
    param_1->f28 = FUN_08a9e824(0xc800);
    param_1->f20 = FUN_08a9e824(0x2800);
    param_1->f24 = FUN_08a9e824(0x2800);
    unsigned int count = 0;
    for (Node *n = param_1->list.next;
         n != (Node *)&param_1->list && count < ((n->val->flags & 0x20000) ? 10U : 20U);
         n = n->next) {
        count++;
        FUN_088d42c4(n->val, param_1->f28, param_1->f20, param_1->f24, &param_1->f2c, 0xa00);
    }
    FUN_088f4ad0(DAT_08beee0c, param_1->f28, param_1->f2c);
}

// Near-miss note: List sentinel hoisted in s3 and conditional branch vs movz for 10/20 limit.
// near miss from lane oc-lbp-20261121 (unreviewed)
