/* LBPMATCH module=EBOOT.BIN start=0x08943ae8 end=0x08943bd0 symbol=FUN_08943ae8 lang=c++ */
/* Hand-written reconstruction from the disassembly (lane api-lbp-a-20261121, Claude Sonnet 5.5); exact match verified; names and types await human review. */
struct Mutex { int a; };
extern Mutex DAT_08bef8f4;
struct Obj { void (**vt)(Obj *); char pad[96]; };
struct Pool {
    Obj *base; Obj **stack; unsigned cap; int top; int heap_count;
    bool owns(Obj *p) { return p >= base && p < base + cap; }
    void put(Obj *p) { top--; stack[top] = p; }
};
extern Pool DAT_08beda98;
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
extern "C" void FUN_08943ae8(char *x)
{
    Obj *p = (Obj *)(x - 68);
    FUN_08ad3088(&DAT_08bef8f4);
    if (DAT_08beda98.owns(p)) {
        DAT_08beda98.put(p);
        destroy(p);
    } else {
        free_heap(&DAT_08beda98, p);
    }
    FUN_08ad2810(&DAT_08bef8f4);
}
