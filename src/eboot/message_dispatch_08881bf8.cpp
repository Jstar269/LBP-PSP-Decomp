/* LBPMATCH module=EBOOT.BIN start=0x08881bf8 end=0x08881d98 symbol=FUN_08881bf8 lang=c++ rodata=0x08bb0ae8 */
/* Hand-written reconstruction from the disassembly (lane lbp21o, Claude Opus 5.5); exact match verified; names and types await human review. */
extern "C" void FUN_08ad3088(void *m);
extern "C" void FUN_08ad2810(void *m);
extern "C" int DAT_08bef8f4;
struct RefObj {
    virtual void f0();
    virtual void destroy();
    int rc;
};
struct Ref {
    RefObj *p;
    Ref() : p(0) {}
    ~Ref() {
        RefObj *q = p;
        if (q) {
            FUN_08ad3088(&DAT_08bef8f4);
            int n = --q->rc;
            FUN_08ad2810(&DAT_08bef8f4);
            if (n == 0) q->destroy();
        }
    }
};
struct Holder { char pad[48]; char list[4]; Ref current; };
extern "C" void *DAT_08bedd04;
extern "C" Holder *DAT_08bedc90;
extern "C" void *DAT_08bedc38;
extern "C" void *DAT_08bedc30;
extern "C" void *DAT_08bedc34;
extern "C" void *DAT_08bedfa8;
extern "C" void FUN_08890c4c(void *o);
extern "C" void FUN_08859c80(void *list, Ref *out);
extern "C" void FUN_08886c2c(void *o);
extern "C" void FUN_08897590(void *o);
extern "C" void FUN_088990d8(void *o);
extern "C" void FUN_0887ff4c(void *o);
extern "C" void FUN_08892928(void *o);
struct State {
    unsigned char a;
    unsigned char b;
    short pad;
    int c;
    int d;
    void reset() { d = 0; c = 0; b = 0; a = 0; }
};
struct Obj {
    int f0;
    State state;
    int pad10[3];
    char f1c[20];
    int *buf0;
    int f34;
    int f38;
    int *buf1;
    int f40;
    int f44;
    int f48;
    int f4c;
    int f50;
    int f54;
};
extern "C" void FUN_08881bf8(Obj *o, int msg)
{
    switch (msg) {
    case 4:
        FUN_08890c4c(DAT_08bedd04);
        break;
    case 8: {
        Ref r;
        FUN_08859c80(DAT_08bedc90->list, &r);
        break;
    }
    case 5: {
        Ref r;
        r.p = DAT_08bedc90->current.p;
        DAT_08bedc90->current.p = 0;
        break;
    }
    case 9:
        FUN_08886c2c(DAT_08bedc38);
        break;
    case 10:
        FUN_08897590(DAT_08bedc30);
        break;
    case 13:
        FUN_088990d8(DAT_08bedc34);
        break;
    case 1:
        o->state.reset();
        break;
    case 20:
        if (o->buf0) delete[] o->buf0;
        o->buf0 = 0;
        o->f34 = 0;
        o->f38 = 0;
        if (o->buf1) delete[] o->buf1;
        o->buf1 = 0;
        o->f40 = 0;
        o->f48 = 0;
        o->f44 = 0;
        o->f4c = 0;
        o->f50 = 0;
        o->f54 = 0;
        break;
    case 3:
        FUN_0887ff4c(o->f1c);
        break;
    case 6:
        FUN_08892928(DAT_08bedfa8);
        break;
    }
}
