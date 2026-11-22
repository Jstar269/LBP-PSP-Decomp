/* LBPMATCH module=EBOOT.BIN start=0x089431d8 end=0x089432b8 symbol=FUN_089431d8 lang=c++ */
/* Hand-written reconstruction from the disassembly (lane api-lbp-a-20261121, Claude Sonnet 5.5); near miss, NOT exact; names and types await human review. */
struct Mutex { int a; };
extern Mutex DAT_08bef8f4;
struct Obj { void (**vt)(Obj *); char pad[456]; };
struct Pool {
    Obj *base; Obj **stack; unsigned cap; int top; int heap_count;
    bool owns(Obj *p) { return p >= base && p < base + cap; }
    void put(Obj *p) { top--; stack[top] = p; }
};
extern Pool DAT_08bef828;
extern "C" void FUN_08ad3088(void *);
extern "C" void FUN_08ad2810(void *);
extern "C" void FUN_08adb8a4(void *p);
inline void destroy(Obj *p) { p->vt[0](p); }
inline void free_heap(Pool *pool, Obj *p)
{
    if (p) {
        pool->heap_count--;
        destroy(p);
        FUN_08adb8a4(p);
    }
}
extern "C" void FUN_089431d8(char *x)
{
    Obj *p = (Obj *)(x - 16);
    FUN_08ad3088(&DAT_08bef8f4);
    if (DAT_08bef828.owns(p)) {
        DAT_08bef828.put(p);
        destroy(p);
    } else {
        free_heap(&DAT_08bef828, p);
    }
    FUN_08ad2810(&DAT_08bef8f4);
}
// NEAR 47 (60 vs 56 words): same pool-release source as the EXACT pool_free_* siblings with 460-byte objects; retail computes base + cap*460 with 'li v0,460; mtlo a1; madd v1,v0; mflo v0' where ours emits a shift/add chain (sll 3/5, subu, sll 2, addu). Compiler multiply-by-constant cost/madd difference (as 0x089f676c, 0x08a1a3e0), not a source difference.
// near miss from lane api-lbp-a-20261121 (unreviewed)
