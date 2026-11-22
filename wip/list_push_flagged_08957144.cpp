/* LBPMATCH module=EBOOT.BIN start=0x08957144 end=0x089571f4 symbol=FUN_08957144 lang=c++ */
/* Hand-written reconstruction from the disassembly (lane api-lbp-a-20261121, Claude Sonnet 5.5); near miss, NOT exact; names and types await human review. */
inline void *operator new(unsigned, void *p) throw() { return p; }
struct Obj { int f0; int rc; };
struct Ref {
    Obj *p;
    Ref(const Ref &o)
    {
        Obj *q = o.p;
        p = q;
        if (q)
            ++q->rc;
    }
};
struct Node { void *next; void *prev; Ref item; };
struct Heap { char pad[1]; };
struct Src { char pad[28]; unsigned flags; };
struct Holder { Src *s; };
extern "C" void *FUN_08adbb84(void *heap, int size);
extern "C" void FUN_08b88fac(void *node, void *pos);
extern "C" void *DAT_08be84a0;
extern "C" void FUN_08957144(Ref *src, char *base)
{
    int idx = 0;
    unsigned f = ((Holder *)src)->s->flags;
    if ((f & 2) == 0) {
        idx = 1;
        if ((f & 4) == 0) {
            idx = 2;
            if ((f & 8) == 0)
                idx = (f & 0x10) ? 3 : 4;
        }
    }
    char *pos = base + idx * 8 + 48;
    Node *n = (Node *)FUN_08adbb84(DAT_08be84a0, 12);
    new (&n->item) Ref(*src);
    FUN_08b88fac(n, pos);
}
// NEAR 21 (44 of 44 words, same instruction set): ternary for the last flag test gives retail's movz; position computed before the allocation (addu in the jal delay slot) and the inline Ref copy with the null-checked placement new match. Differences are scheduling/regalloc only: retail copies the base pointer into a2 (ours a3) and loads the heap global (lui v0; lw a0,-31584(v0)) AFTER the idx/movz sequence, ours hoists it to the top.
// near miss from lane api-lbp-a-20261121 (unreviewed)
