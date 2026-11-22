/* LBPMATCH module=EBOOT.BIN start=0x088d3670 end=0x088d3710 symbol=FUN_088d3670 lang=c++ */
/* Hand-written reconstruction from the disassembly (lane oc-lbp-20261121, Antigravity); near match; names and types await human review. */

struct PayloadA {
    unsigned char pad1[0xaa];
    unsigned short title[65];
    unsigned short desc[208];
};

struct PayloadB {
    unsigned char pad1[0xaa];
    unsigned short title[65];
    unsigned short desc[208];
};

struct Item {
    unsigned char pad1[0x28];
    int kind;
    unsigned char pad2[0xe0 - 0x2c];
    union {
        PayloadA a;
        PayloadB b;
    } u;

    inline bool is_a()
    {
        return kind == 3 || kind == 5 || kind == 4;
    }
    inline unsigned short *get_buf(int which)
    {
        PayloadB *p = is_a() ? (PayloadB *)&u.a : &u.b;
        return (which == 0) ? p->title : p->desc;
    }
};

extern "C" void FUN_088d3670(Item *param_1, int param_2, unsigned short *param_3)
{
    if (param_1->is_a()) {
        return;
    }
    PayloadB *p = param_1->is_a() ? (PayloadB *)&param_1->u.a : &param_1->u.b;
    unsigned short *dst;
    int cap;
    if (param_2 == 0) {
        dst = p->title;
        cap = 65;
    } else if (param_2 == 1) {
        dst = p->desc;
        cap = 208;
    } else {
        return;
    }
    int i = 0;
    while (*param_3 != 0 && i < cap - 1) {
        *dst++ = *param_3++;
        i++;
    }
    *dst = 0;
}

// Near-miss note: GCC folds identical branch arms; 33/40 words.
// near miss from lane oc-lbp-20261121 (unreviewed)
