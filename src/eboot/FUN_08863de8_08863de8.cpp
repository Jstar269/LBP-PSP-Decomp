/* LBPMATCH module=EBOOT.BIN start=0x08863de8 end=0x08863ebc symbol=FUN_08863de8 lang=c++ */
/* Hand-written reconstruction from the disassembly (lane oc-lbp-20261121, Antigravity); exact match verified; names and types await human review. */

inline void *operator new(unsigned, void *p) throw() { return p; }
struct Mutex { int a; };
extern Mutex DAT_08bef8f4;
extern "C" void FUN_08ad3088(void *);
extern "C" void FUN_08ad2810(void *);
extern "C" void *FUN_08adb810(unsigned size, unsigned align, int a, int b);
extern "C" void FUN_088638fc(void *o);
inline void *operator new(unsigned size, unsigned align, int a, int b) { return FUN_08adb810(size, align, a, b); }
struct Obj {
    char pad[228];
    Obj() { FUN_088638fc(this); }
};
struct Pool { Obj *base; Obj **stack; int cap; int top; int heap_count; };
extern Pool DAT_08bedd68;
struct Handle {
    Obj *p;
    Handle() {}
    Handle(const Handle &);
};
inline Obj *make()
{
    Obj *o;
    if (DAT_08bedd68.top < DAT_08bedd68.cap) {
        o = new (DAT_08bedd68.stack[DAT_08bedd68.top++]) Obj;
    } else {
        DAT_08bedd68.heap_count++;
        o = new (16, 0, 0) Obj;
    }
    return o;
}
extern "C" Handle FUN_08863de8()
{
    Handle h;
    FUN_08ad3088(&DAT_08bef8f4);
    FUN_08ad3088(&DAT_08bef8f4);
    Obj *o = make();
    FUN_08ad2810(&DAT_08bef8f4);
    h.p = o;
    FUN_08ad2810(&DAT_08bef8f4);
    return h;
}
