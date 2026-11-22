/* LBPMATCH module=EBOOT.BIN start=0x088ec854 end=0x088ec8f4 symbol=FUN_088ec854 lang=c++ */
/* Hand-written reconstruction from the disassembly (lane oc-lbp-20261121, Antigravity); near match; names and types await human review. */

struct Lock {
    int x;
};

struct Src {
    int f0;
    int f4;
    int f8;
    int *ref;
};

struct Obj {
    void *vt;
    short f4;
    short f6;
    int f8;
    int f0c;
    int f10;
    int f14;
    int *ref;
};

extern "C" Lock DAT_08bef8f4;
extern "C" void *DAT_08bb4cc8[];
extern "C" void FUN_08ad3088(Lock *);
extern "C" void FUN_08ad2810(Lock *);

extern "C" void FUN_088ec854(Obj *param_1, int param_2, Src *param_3)
{
    param_1->f4 = -1;
    param_1->f6 = 1;
    param_1->vt = DAT_08bb4cc8;
    param_1->f8 = param_2;
    param_1->f0c = param_3->f0;
    param_1->f10 = param_3->f4;
    param_1->f14 = param_3->f8;
    int *r = param_3->ref;
    param_1->ref = r;
    if (r != 0) {
        FUN_08ad3088(&DAT_08bef8f4);
        (*r)++;
        FUN_08ad2810(&DAT_08bef8f4);
    }
    param_1->f4 = 0x1c;
}

// Near-miss note: Register/scheduling diff on 40/40 words.
// near miss from lane oc-lbp-20261121 (unreviewed)
