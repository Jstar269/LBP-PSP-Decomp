/* LBPMATCH module=EBOOT.BIN start=0x08849fc8 end=0x08849ff0 symbol=FUN_08849fc8 lang=c++ cc=gcc402x-O1 */
/* Hand-written reconstruction (external matching lane lbp4c); exact match verified under cc=gcc402x-O1 after a compiler-profile sweep (lane lbp20s, Claude Opus 5.5); names and types await human review. */
struct Obj { virtual void d0(); virtual void d1(); virtual void f(); };
struct Holder { Obj *o; };
extern "C" void FUN_08849fc8(Holder *h) { h->o->f(); }
