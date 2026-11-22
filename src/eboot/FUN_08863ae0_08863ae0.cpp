/* LBPMATCH module=EBOOT.BIN start=0x08863ae0 end=0x08863bc8 symbol=FUN_08863ae0 lang=c++ */
/* Hand-written reconstruction from the disassembly (lane oc-lbp-20261121, Antigravity); exact match verified; names and types await human review. */

struct Mutex { int a; };
extern Mutex DAT_08bef8f4;
struct Obj { void (**vt)(Obj *); char pad[224]; };
struct Pool {
    Obj *base; Obj **stack; unsigned cap; int top; int heap_count;
    bool owns(Obj *p) { return p >= base && p < base + cap; }
    void put(Obj *p) { top--; stack[top] = p; }
};
extern Pool DAT_08bedd68;
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
extern "C" void FUN_08863ae0(char *x)
{
    Obj *p = (Obj *)(x - 8);
    FUN_08ad3088(&DAT_08bef8f4);
    if (DAT_08bedd68.owns(p)) {
        DAT_08bedd68.put(p);
        destroy(p);
    } else {
        free_heap(&DAT_08bedd68, p);
    }
    FUN_08ad2810(&DAT_08bef8f4);
}
