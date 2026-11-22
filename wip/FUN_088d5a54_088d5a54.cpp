/* LBPMATCH module=EBOOT.BIN start=0x088d5a54 end=0x088d5b38 symbol=FUN_088d5a54 lang=c++ */
/* Hand-written reconstruction from the disassembly (lane oc-lbp-20261121, Antigravity); near match; names and types await human review. */

struct Sub {
    unsigned int id;
    int pad[4];
    unsigned char dirty;
};

struct Mgr {
    unsigned char pad[0x1c8];
    Sub *sub;
};

struct Net {
    unsigned char pad[0x54];
    unsigned char f54;
};

struct Item {
    unsigned int flags;
    int f04;
    int f08;
    int f0c;
    int f10;
    unsigned short f14;
    unsigned short pad16;
    int pad18[2];
    unsigned long long ts;
    int kind;
    int pad2c[17];
    unsigned int f70;
    int f74;
    int f78;
    short f7c;
    short pad7e;
    short pad80;
    unsigned short f82;
    unsigned char pad84[10];
    unsigned char f8e;
    unsigned char f8f;
};

extern "C" Mgr *DAT_08beed14;
extern "C" Net *DAT_08beed8c;
extern "C" unsigned long long FUN_088d1eb8(Sub *);
extern "C" void FUN_088d58d4(Item *);
extern "C" void FUN_088d2480(Sub *, Item *);
extern "C" void FUN_088d1e30(Sub *, unsigned char);

extern "C" void FUN_088d5a54(Item *param_1)
{
    param_1->kind = 1;
    param_1->f7c = -1;
    param_1->ts = FUN_088d1eb8(DAT_08beed14->sub);
    Sub *sub = DAT_08beed14->sub;
    param_1->f82 = 0xff;
    unsigned int id = ++sub->id;
    sub->dirty = 1;
    param_1->f70 = id;
    param_1->flags = (param_1->flags & 0xff1f8fce) | 0x20000;
    param_1->f78 = 0;
    param_1->f8e = 0;
    param_1->f8f = 0;
    param_1->f0c = 0;
    param_1->f08 = 0;
    param_1->f10 = 0;
    param_1->f14 = 0;
    FUN_088d58d4(param_1);
    Sub *s2 = DAT_08beed14->sub;
    DAT_08beed8c->f54 = 1;
    FUN_088d2480(s2, param_1);
    FUN_088d1e30(DAT_08beed14->sub, 0);
}

// Near-miss note: Branch/register scheduling diff on 57/57 words.
// near miss from lane oc-lbp-20261121 (unreviewed)
