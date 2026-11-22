/* LBPMATCH module=EBOOT.BIN start=0x088b6ba8 end=0x088b6c88 symbol=FUN_088b6ba8 lang=c++ */
/* Hand-written reconstruction from the disassembly (lane oc-lbp-20261121, Antigravity); exact match verified; names and types await human review. */

struct Msg {
    unsigned char pad1[0x2c];
    void *f2c;
    int f30;
    int f34;
};

struct Obj2 {
    unsigned char pad[0xd8];
    unsigned char f0d8;
    unsigned char pad2[3];
    int f0dc;
};

struct Dummy {
};

struct Str;

extern "C" void FUN_088b6b3c(void *, int, const Dummy *);
extern "C" void FUN_08970b6c(void *);
extern "C" void FUN_0896fb74(void *, const Str *);

struct Str {
    void *p;
    inline Str(int id, const Dummy &d = Dummy())
    {
        FUN_088b6b3c(this, id, &d);
    }
    inline ~Str()
    {
        if (p != 0) {
            FUN_08970b6c(p);
        }
    }
};

struct Mgr {
    inline void notify(const Str &s)
    {
        FUN_0896fb74(this, &s);
    }
};

extern "C" Mgr *DAT_08bf03b8;
extern "C" void *DAT_08bedb54;
extern "C" void FUN_08a9e0c4(void *);
extern "C" void FUN_08842808(void *, int, unsigned int);

extern "C" void FUN_088b6ba8(Msg **param_1, Obj2 *param_2)
{
    void *p = (*param_1)->f2c;
    int err = (*param_1)->f34;
    if (p != 0) {
        FUN_08a9e0c4(p);
    }
    if (err != 0) {
        DAT_08bf03b8->notify(Str(0x84d2));
    } else {
        param_2->f0dc = 0;
        FUN_08842808(DAT_08bedb54, 1, param_2->f0d8);
        DAT_08bf03b8->notify(Str(0x84d4));
    }
}
