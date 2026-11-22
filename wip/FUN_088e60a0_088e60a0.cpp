/* LBPMATCH module=EBOOT.BIN start=0x088e60a0 end=0x088e6184 symbol=FUN_088e60a0 lang=c++ */
/* Hand-written reconstruction from the disassembly (lane oc-lbp-20261121, Antigravity); near match; names and types await human review. */

extern "C" void FUN_088e301c(void *, unsigned char);
extern "C" void FUN_08b9eb88(void *, int, int);

struct Node {
    Node *next;
    Node *prev;
};

struct Sub {
    int pad[5];
    Node list;

    inline void init()
    {
        list.next = &list;
        list.prev = &list;
        FUN_088e301c(this, 0);
    }
};

struct Pair {
    int a;
    int b;
};

struct Net {
    Sub s0;
    Sub s1;
    Sub s2;
    unsigned char f54;
    unsigned char f55;
    unsigned char f56;
    unsigned char f57;
    unsigned char f58;
    unsigned char pad59[3];
    int f5c;
    unsigned char f60;
    unsigned char pad61[3];
    Node list2;
    Pair p;
    int f74;
    unsigned char buf1[0x14];
    int f8c;
    unsigned char f90;
    unsigned char pad91;
    unsigned char buf2[0x80];
    unsigned char pad112[0x192 - 0x112];
    unsigned char buf3[0x22];
};

extern "C" void FUN_088e60a0(Net *param_1)
{
    param_1->s0.init();
    param_1->s1.init();
    param_1->s2.init();
    param_1->f5c = -1;
    Pair *p = &param_1->p;
    p->b = 0;
    param_1->list2.prev = &param_1->list2;
    param_1->f54 = 0;
    param_1->f90 = 0xff;
    param_1->f55 = 0;
    param_1->f56 = 0;
    param_1->f57 = 0;
    param_1->f58 = 0;
    param_1->f60 = 0;
    param_1->list2.next = &param_1->list2;
    p->a = 0;
    param_1->f74 = 0;
    param_1->f8c = 0;
    FUN_08b9eb88(param_1->buf1, 0, 0x14);
    FUN_08b9eb88(param_1->buf3, 0, 0x22);
    p->a = 0;
    p->b = 0;
    FUN_08b9eb88(param_1->buf2, 0, 0x80);
}

// Near-miss note: Register/branch layout diff on 59/57 words.
// near miss from lane oc-lbp-20261121 (unreviewed)
