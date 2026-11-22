/* LBPMATCH module=EBOOT.BIN start=0x088f2124 end=0x088f21ec symbol=FUN_088f2124 lang=c++ */
/* Hand-written reconstruction from the disassembly (lane oc-lbp-20261121, Antigravity); exact match verified; names and types await human review. */

extern "C" void FUN_08a9c014(void *);
extern "C" void FUN_08a9bff8(void *);

struct Str {
    int pad[6];
    inline Str() { FUN_08a9c014(this); }
    inline ~Str() { FUN_08a9bff8(this); }
};

struct Mgr {
    unsigned char pad[0x1fc];
    unsigned int b0 : 1;
    bool b1 : 1;
    unsigned int rest : 30;
};

struct Loc {
    unsigned char pad[0x29c];
    void *f29c;
};

struct Ui {
    unsigned char pad[0xa8];
    int fa8;
};

extern "C" Mgr *DAT_08beed14;
extern "C" Loc *DAT_08be85fc;
extern "C" bool FUN_0888255c(void);
extern "C" void FUN_088dfd64(Str *, void *);
extern "C" void FUN_088f1f9c(Ui *, Str *);

extern "C" bool FUN_088f2124(Ui *param_1, int param_2)
{
    if (FUN_0888255c() && !DAT_08beed14->b1) {
        if (param_2 != param_1->fa8) {
            param_1->fa8 = param_2;
        }
        if (!DAT_08beed14->b0) {
            Str s;
            FUN_088dfd64(&s, DAT_08be85fc->f29c);
            FUN_088f1f9c(param_1, &s);
            DAT_08beed14->b0 = 1;
        }
        return true;
    }
    return false;
}
