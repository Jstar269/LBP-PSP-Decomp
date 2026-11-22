/* LBPMATCH module=EBOOT.BIN start=0x088b63c8 end=0x088b6480 symbol=FUN_088b63c8 lang=c99 */
/* Hand-written reconstruction from the disassembly (lane oc-lbp-20261121, Antigravity); exact match verified; names and types await human review. */

struct Pair {
    int a;
    int b;
};

struct Target {
    void (**vt)(struct Target *, struct Pair *, int *, int, int);
};

struct Msg {
    int pad1[4];
    int a;
    int b;
    int pad2[4];
    int f28;
    int f2c;
    int f30;
    int f34;
    unsigned char pad3[0x90 - 0x38];
    struct Target *target;
};

struct Obj2 {
    unsigned char pad[0x64];
    void *f64;
};

extern void FUN_088050b8(void *);

void FUN_088b63c8(struct Msg **param_1, struct Obj2 *param_2)
{
    struct Msg *m = *param_1;
    int f2c = m->f2c;
    struct Pair p1;
    struct Pair p2;
    struct Target *t = m->target;
    int a = m->a;
    int b = m->b;

    if (m->f34 == 0) {
        p2.a = a;
        p2.b = b;
        t->vt[6](t, &p2, &f2c, m->f28, 1);
    } else {
        p1.a = a;
        p1.b = b;
        t->vt[6](t, &p1, &f2c, m->f28, 0);
        if (param_2 != 0) {
            void *x = param_2->f64;
            param_2->f64 = 0;
            if (x != 0) {
                FUN_088050b8(x);
            }
        }
    }
}

// Remaining diffs (45/46 words; 36 differ at +0x1c): GCC hoists target->vt load before the branch, whereas retail reloads target->vt inside each branch after storing Pair on the stack; C++ virtual method gave same 45/46 words.
// near miss from lane oc-lbp-20261121 (unreviewed)
