/* LBPMATCH module=EBOOT.BIN start=0x089460d8 end=0x08946168 symbol=FUN_089460d8 lang=c99 */
/* Hand-written reconstruction from the disassembly (lane api-lbp-a-20261121, Claude Sonnet 5.5); near miss, NOT exact; names and types await human review. */
extern void FUN_08944bf0(void *);
extern void FUN_08a9c304(void *, void *);
extern void FUN_0894a258(void *);
extern void *FUN_08b9eb88(void *, int, int);
extern char DAT_08bb7270, DAT_08bb7350, DAT_08bb7364;
struct Slice { int a, b; };
struct D {
    void *vt;
    int pad1[17];
    int name[6];
    int f96, f100;
    void *vt2;
    int f108;
    int f112, f116;
    char buf[288];
    char *pbuf;
};
void *FUN_089460d8(struct D *o)
{
    struct Slice s;
    FUN_08944bf0(o);
    o->vt = &DAT_08bb7270;
    s.a = 0;
    s.b = 0;
    FUN_08a9c304(o->name, &s);
    o->f96 = 0;
    o->f100 = 0;
    FUN_0894a258(&o->vt2);
    o->vt = &DAT_08bb7350;
    o->f112 = 0;
    o->f116 = 0;
    o->vt2 = &DAT_08bb7364;
    o->pbuf = o->buf;
    return FUN_08b9eb88(o->buf, 0, 288);
}
// NEAR 14 (35 of 36 words): layout/offsets right (base ctor FUN_08944bf0, name init via FUN_08a9c304(&name,&{0,0}), base2 ctor at +104, vtables, memset of buf at +120). Retail tail-calls the final memset (j 0x8b9eb88) although the {0,0} local's address was passed to a call, and keeps the zero constants of the +112/+116 stores in a0/a1 (move a0,zero; move a1,zero; sw a0,112; sw a1,116); ours has jal + epilogue and sw zero.
// Possible cause: ours refuses the sibcall because of the addressable local; tried a C++ temporary Slice() with the same result.
// near miss from lane api-lbp-a-20261121 (unreviewed)
