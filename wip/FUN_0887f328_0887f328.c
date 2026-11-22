/* LBPMATCH module=EBOOT.BIN start=0x0887f328 end=0x0887f3d0 symbol=FUN_0887f328 lang=c99 */
/* Hand-written reconstruction from the disassembly (lane oc-lbp-20261121, Antigravity); near match; names and types await human review. */

extern char DAT_08bb0a60[];

extern void *FUN_08a9e838(int);
extern void FUN_0885daa0(void *, char *, void *);

struct Sub {
    int mode;
    int a;
    int b;
    int e;
    int d;
    int g;
};

struct In {
    int f0;
    struct Sub sub;
};

struct Args {
    int a;
    int b;
    int c;
    int d;
    int e;
    int f;
    int g;
    int h;
    int mode;
};

void *FUN_0887f328(struct In *param_1)
{
    struct Args args;
    struct Sub *sub = &param_1->sub;
    void *p;

    args.a = sub->a;
    args.b = sub->b;
    args.d = sub->d;
    args.e = 0;
    args.f = 0;
    args.g = 0;
    args.mode = param_1->sub.mode;
    args.h = 0;
    args.c = 1;
    switch (args.mode) {
    case 7:
        args.g = sub->g;
        break;
    case 5:
        args.e = 1;
        args.f = sub->e;
        break;
    }
    p = FUN_08a9e838(444);
    FUN_0885daa0(p, DAT_08bb0a60, &args);
    return p;
}

// NEAR: 42/42 words; 2 differ at +0x18..+0x1c.
// Remaining diff: single load swap lw a2,4(a0); lw v1,8(a1) vs retail lw v1,8(a1); lw a2,4(a0).
// Ruled out: C++ version (identical 2 diffs), moving mode load before/after args.d/args.c (4 differ),
// aggregate initializer (emits .data template).
// near miss from lane oc-lbp-20261121 (unreviewed)
