/* LBPMATCH module=EBOOT.BIN start=0x089477cc end=0x08947880 symbol=FUN_089477cc lang=c99 */
/* Hand-written reconstruction from the disassembly (lane api-lbp-a-20261121, Claude Sonnet 5.5); near miss, NOT exact; names and types await human review. */
extern void *FUN_08b9eb88(void *, int, int);
extern void FUN_08baa4fc(void *, void *);
extern void FUN_08ba0868(char *, int, const char *);
extern const char *DAT_08bef8d8;
struct Src { char pad[20]; int f20; char name[1]; };
struct SrcP { struct Src *s; };
struct O {
    int pad0;
    int size;
    int a8, a12;
    int f16, f20, f24, f28;
    char pad1[20];
    int f52;
    char pad2[8];
    char title[16];
    char name[16];
    char pad3[1428];
    int f1524, f1528;
};
void FUN_089477cc(struct O *o, struct SrcP *p)
{
    FUN_08b9eb88(&o->size, 0, 1536);
    o->size = 1536;
    FUN_08baa4fc(&o->a8, &o->a12);
    o->f28 = 31;
    o->f16 = 32;
    o->f24 = 33;
    o->f20 = 34;
    FUN_08ba0868(o->title, 13, DAT_08bef8d8);
    FUN_08ba0868(o->name, 20, (const char *)p->s + 24);
    o->f52 = 11;
    {
        struct Src *q = p->s;
        int f = q->f20;
        o->f1528 = (int)((char *)q + 44);
        o->f1524 = f;
    }
}
// NEAR 3 words: all stores and calls match; only the jal delay slot differs: retail 'addiu a0,s0,8; addiu a1,s0,12; jal 0x8baa4fc; sw v0,4(s0)' (the size store fills the delay slot), ours 'sw v0,4(s0); addiu a0; jal; addiu a1'. Local n for 1536, comma expression, pointer locals and moving the store after the call (12 words) did not change it. Loading q->f20 into a local before the 1528 store reproduces the retail order of the last stores.
// near miss from lane api-lbp-a-20261121 (unreviewed)
