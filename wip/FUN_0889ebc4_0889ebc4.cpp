/* LBPMATCH module=EBOOT.BIN start=0x0889ebc4 end=0x0889ec90 symbol=FUN_0889ebc4 lang=c++ */
/* Hand-written reconstruction from the disassembly (lane oc-lbp-20261121, Antigravity); exact match verified; names and types await human review. */

extern "C" void FUN_0889de48(void *, void *, void *);
extern "C" void FUN_0889d4d4(void *, void *, void *);
extern "C" void FUN_0889d3a4(void *, void *, void *);

struct Vec {
    void *begin;
    void *end;
    void *cap;
};

struct Obj {
    int f00;
    int f04;
    int f08;
    int f0c;
    int f10;
    unsigned char f14;
    unsigned char f15;
    unsigned char f16;
    unsigned char f17;
    int f18;
    int f1c;
    int f20;
    Vec v1;
    Vec v2;
    Vec v3;
    int f48;
    int f4c;
    int f50;
    unsigned char f54;
};

extern "C" void FUN_0889ebc4(Obj *self)
{
    Vec *v1 = &self->v1;
    Vec *v2 = &self->v2;
    Vec *v3 = &self->v3;

    self->f00 = 0;
    self->f04 = 0;
    self->f08 = 0;
    self->f0c = 0;
    self->f10 = 0;
    self->f14 = 0;
    self->f15 = 0;
    self->f16 = 0;
    self->f17 = 0;
    self->f18 = 0;
    self->f1c = 0;
    self->f20 = 0;
    v1->begin = 0;
    v1->end = 0;
    v1->cap = 0;
    v2->begin = 0;
    v2->end = 0;
    v2->cap = 0;
    v3->begin = 0;
    v3->end = 0;
    v3->cap = 0;
    self->f48 = 0;
    self->f4c = 0;
    self->f50 = 0;
    self->f54 = 0;

    FUN_0889de48(v1, v1->begin, v1->end);
    FUN_0889d4d4(v2, v2->begin, v2->end);
    FUN_0889d3a4(v3, v3->begin, v3->end);
}

// Remaining diffs: exact size (51/51 words) but GCC schedules the zero stores to f00..f54 in a different order than retail (tried placement-new Obj() which grew to 58 words).
// near miss from lane oc-lbp-20261121 (unreviewed)
