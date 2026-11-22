/* LBPMATCH module=EBOOT.BIN start=0x0894af44 end=0x0894b020 symbol=FUN_0894af44 lang=c++ */
/* Hand-written reconstruction from the disassembly (lane api-lbp-a-20261121, Claude Sonnet 5.5); exact match verified; names and types await human review. */
inline void *operator new(unsigned, void *p) throw() { return p; }
struct Pair { int a; int b; };
struct VecB { unsigned char *start; unsigned char *finish; unsigned char *eos; };
struct VecP { Pair *start; Pair *finish; Pair *eos; };
extern "C" void FUN_0894af10(VecB *v, unsigned n, const unsigned char *x);
extern "C" void *memcpy(void *, const void *, unsigned);
extern "C" void FUN_0894aae0(VecP *v, Pair *pos, const Pair *x);
inline void push_back(VecP *v, const Pair &x)
{
    if (v->finish != v->eos) {
        new (v->finish) Pair(x);
        ++v->finish;
    } else
        FUN_0894aae0(v, v->finish, &x);
}
struct Buf { int pad0, pad1; VecB bytes; VecP runs; };
extern "C" void FUN_0894af44(Buf *b, const void *src, int n, int m)
{
    int sz = n * m;
    unsigned char z = 0;
    VecB *v = &b->bytes;
    unsigned off = v->finish - v->start;
    FUN_0894af10(v, off + sz, &z);
    memcpy(v->start + off, src, sz);
    Pair p;
    p.a = n;
    p.b = m;
    push_back(&b->runs, p);
}
// NEAR: 9 words differ, only the prologue scheduling of `mult a2,a3`/`mflo s3` (ours: mult before addiu sp, mflo after the s1 move; retail: addiu sp, mult, ..., mflo right after sw s3).
// Tried: n*m inline vs `int sz` at top / after z / after off / before the first call, m*n, unsigned params, cc=gcc404xs-O2 (28 differ).
// Matching idioms found: `VecB *v = &b->bytes` local pointer (sub-object also passed to the call); push_back as `new (v->finish) Pair(x); ++v->finish;` (finish reloaded).
// near miss from lane api-lbp-a-20261121 (unreviewed)
