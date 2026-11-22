/* LBPMATCH module=EBOOT.BIN start=0x08883a64 end=0x08883b08 symbol=FUN_08883a64 lang=c++ */
/* Hand-written reconstruction from the disassembly (lane oc-lbp-20261121, Antigravity); near match; names and types await human review. */

struct Sub {
    int f0;
    volatile int state;
};

struct Base {
    bool (**vt)(void *);
    int f4;
    int f8;
};

struct Obj : Base, Sub {
};

struct Ptr {
    Obj *p;
};

extern "C" Ptr DAT_08bedfb4;
extern "C" void FUN_088829e8(void) __attribute__((weak));
extern "C" int FUN_08849ff0(Obj *);

static inline bool is_ready()
{
    return DAT_08bedfb4.p != 0 && FUN_088829e8 && FUN_08849ff0(DAT_08bedfb4.p) != 0;
}

extern "C" bool FUN_08883a64(void)
{
    if (is_ready()) {
        return DAT_08bedfb4.p->vt[11](DAT_08bedfb4.p);
    }
    if (DAT_08bedfb4.p == 0) {
        return true;
    }
    Sub *s = DAT_08bedfb4.p;
    if (s->state >= 4) {
        return true;
    }
    return s->state == 0;
}

// NEAR: 37/41 words; 21 differ; first at +0x10.
// Remaining diff: retail reloads DAT_08bedfb4 at +0x7c even when initial DAT_08bedfb4==0
// and uses andi v0,v1,0xff for the bool return; ours jump-threads DAT_08bedfb4==0 straight to return 1.
// Ruled out: volatile on DAT_08bedfb4 (46/41 words, 38 differ), direct inline condition without helper (33 differ).
// near miss from lane oc-lbp-20261121 (unreviewed)
