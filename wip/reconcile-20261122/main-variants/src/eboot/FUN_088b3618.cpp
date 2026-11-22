/* LBPMATCH module=EBOOT.BIN start=0x088b3618 end=0x088b36c0 symbol=FUN_088b3618 lang=c++ */
/* Hand-written reconstruction from the disassembly (lane lbp17c, Codex GPT-6); exact match verified; names and types await human review. */
struct Item {
    virtual void v0(); virtual void v1(); virtual void v2(); virtual void v3(); virtual void v4();
    virtual void v5(); virtual void v6(); virtual void v7(); virtual void v8(); virtual void v9();
};
struct IFace { int pad[2]; unsigned char b8; char pad2[8]; unsigned char b17; };
struct H { IFace *p; H(const H &); };
struct Obj {
    virtual void m0(); virtual void m1(); virtual void m2(); virtual void m3(); virtual void m4();
    virtual void m5(); virtual void m6(); virtual void m7(); virtual void m8(); virtual void m9();
    virtual void m10(); virtual void m11(); virtual void m12(); virtual void m13(); virtual H m14();
};
extern "C" int FUN_08ad2cf0(void *);
extern "C" void FUN_08ad3088(void *);
extern "C" void FUN_08ad2810(void *);
extern "C" void FUN_08804f3c(H *);
extern "C" void FUN_08804f34() __attribute__((weak));

extern "C" void FUN_088b3618(Obj *self)
{
    if (*((unsigned char *)self + 0xa9) != 0) {
        if (FUN_08ad2cf0((char *)self + 20) != 0)
            return;
    } else {
        FUN_08ad3088((char *)self + 20);
    }
    H h = self->m14();
    if (h.p != 0 && FUN_08804f34 != 0 && h.p->b17 == 0)
        ((Item *)h.p)->v9();
    FUN_08ad2810((char *)self + 20);
    FUN_08804f3c(&h);
}


