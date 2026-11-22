/* LBPMATCH module=EBOOT.BIN start=0x088c2380 end=0x088c2444 symbol=FUN_088c2380 lang=c++ */
/* Hand-written reconstruction from the disassembly (lane oc-lbp-20261121, Antigravity); exact match verified; names and types await human review. */

extern "C" void *DAT_08bb3948[];
extern "C" void FUN_088b2dd0(void *, int, int);
extern "C" void FUN_088b5868(void *);
extern "C" void FUN_08b9eb88(void *, int, int);

struct NodeBase {
    int color;
    NodeBase *parent;
    NodeBase *left;
    NodeBase *right;
};

struct Tree {
    int pad;
    NodeBase header;
    int count;
    inline void init()
    {
        FUN_08b9eb88(&header, 0, 16);
        header.color = 0;
        header.parent = 0;
        header.left = &header;
        header.right = &header;
        count = 0;
    }
};

struct SubTree {
    int data[6];
};

struct Obj {
    void **vt;
    int pad[4];
    void *f14;
    unsigned char f18;
    unsigned char f19;
    unsigned char f1a;
    unsigned char f1b;
    int f1c;
    int f20;
    SubTree t1;
    Tree t2;
    SubTree t3;
    void *arr[8];
    int f8c;
    int f90;
};

extern "C" void FUN_088c2380(Obj *self, void *a, int b)
{
    FUN_088b2dd0(self, 4, b);
    self->f1a = 1;
    self->f19 = 1;
    self->vt = DAT_08bb3948;
    self->f14 = a;
    self->f18 = 0;
    self->f1c = 0;
    self->f20 = 0;
    FUN_088b5868(&self->t1);
    self->t2.init();
    FUN_088b5868(&self->t3);
    void **p = self->arr;
    int i = 7;
    do {
        i--;
        *p = 0;
        p++;
    } while (i != -1);
    self->f90 = 0;
    self->f8c = 0;
}

// Remaining diffs (51/49 words; 50 differ): GCC spills an extra s-reg (sp,-32) whereas retail fits in sp,-16 (s0..s2) via Obj constructor member initialization.
// near miss from lane oc-lbp-20261121 (unreviewed)
