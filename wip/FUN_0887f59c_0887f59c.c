/* LBPMATCH module=EBOOT.BIN start=0x0887f59c end=0x0887f644 symbol=FUN_0887f59c lang=c99 */
/* Hand-written reconstruction from the disassembly (lane oc-lbp-20261121, Antigravity); near match; names and types await human review. */

extern void FUN_0887eef8(void) __attribute__((weak));

struct Sub {
    void (**vt)(struct Sub *);
};

struct Item {
    int pad[3];
    struct Sub sub;
};

struct Slot1 {
    struct Item *p;
    unsigned char pad[44];
};

struct Slot2 {
    struct Item *p;
    unsigned char pad[48];
};

struct Obj {
    unsigned char pad1[44];
    int state;
    unsigned char pad2[144 - 48];
    struct Slot1 arr1[23];
    unsigned char pad3[1432 - 144 - 23 * 48];
    struct Slot2 arr2[23];
};

void FUN_0887f59c(struct Obj *self)
{
    int i;
    self->state = 1;
    for (i = 0; i < 23; i++) {
        struct Item *p = self->arr1[i].p;
        if (p != 0 && FUN_0887eef8) {
            p->sub.vt[3](&p->sub);
        }
        p = self->arr2[i].p;
        if (p != 0 && FUN_0887eef8) {
            p->sub.vt[3](&p->sub);
        }
    }
}

// NEAR: 42/42 words; 5 differ; first at +0x14.
// Remaining diff: induction variable base offset (ours keeps s1=self, s0=self with 144(s1)/1432(s0);
// retail initializes s1=self+144, s0=self+1432 with 0(s1)/0(s0)).
// Ruled out: local arr1/arr2 pointers in C99 & C++ (fixes offset to 0 but swaps s0/s1 -> 7 differ),
// explicit p1++/p2++ pointer loop (11 differ), C++ indexed loop (5 differ).
// near miss from lane oc-lbp-20261121 (unreviewed)
