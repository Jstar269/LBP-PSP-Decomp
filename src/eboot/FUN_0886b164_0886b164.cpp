/* LBPMATCH module=EBOOT.BIN start=0x0886b164 end=0x0886b250 symbol=FUN_0886b164 lang=c++ */
/* Hand-written reconstruction from the disassembly (lane oc-lbp-20261121, Antigravity); exact match verified; names and types await human review. */

extern int DAT_08bb0150;
extern int DAT_08bb0188;
extern int DAT_08bb00f0;

extern "C" void FUN_0886a99c(void);
extern "C" void FUN_08879430(void *, void *, void (*)(void), int);
extern "C" void *FUN_08b9eb88(void *, int, unsigned);
extern "C" void FUN_0885d290(void *, int, int, int);

struct Node {
    int color;
    Node *parent;
    Node *left;
    Node *right;
};

struct Tree {
    int pad;
    Node header;
    int count;
};

struct Obj {
    void *vt0;
    int pad1[2];
    void *vt1;
    unsigned char pad2[200 - 16];
    Tree t1;
    Tree t2;
    void *sub_vt;
    unsigned char pad3[456 - 252];
    int f456;
    int f460;
    int f464;
};

extern "C" void FUN_0886b164(Obj *self, void *param_2)
{
    FUN_08879430(self, param_2, FUN_0886a99c, 3);
    self->vt0 = &DAT_08bb0150;
    self->vt1 = &DAT_08bb0188;
    Tree *t1 = &self->t1;
    Node *h1 = &t1->header;
    FUN_08b9eb88(h1, 0, 16);
    t1->header.color = 0;
    t1->header.parent = 0;
    t1->header.left = h1;
    t1->header.right = h1;
    t1->count = 0;
    Tree *t2 = &self->t2;
    Node *h2 = &t2->header;
    FUN_08b9eb88(h2, 0, 16);
    t2->header.color = 0;
    t2->header.parent = 0;
    t2->header.left = h2;
    t2->header.right = h2;
    t2->count = 0;
    FUN_0885d290(&self->sub_vt, 4, 1, 5);
    self->sub_vt = &DAT_08bb00f0;
    self->f456 = 0;
    self->f460 = 0;
    self->f464 = 0;
}
