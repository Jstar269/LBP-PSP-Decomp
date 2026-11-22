/* LBPMATCH module=EBOOT.BIN start=0x08867064 end=0x08867164 symbol=FUN_08867064 lang=c99 */
/* Hand-written reconstruction from the disassembly (lane oc-lbp-20261121, Antigravity); near match; names and types await human review. */

extern int DAT_08bef8f4;
extern void *DAT_08bedba4;

extern void FUN_08866d9c(void *, void *);
extern void FUN_08866a7c(void *, void *, void *, int);
extern void FUN_08ad3088(int *);
extern void FUN_08ad2810(int *);
extern void FUN_0887c328(void *, void *);
extern void FUN_08866590(void *);

struct Ref {
    void (**vt)(struct Ref *);
    int count;
};

struct Sub {
    int a;
    int state;
};

struct Resp {
    int a;
    int code;
};

struct Obj {
    unsigned char pad1[0x114];
    int f114;
    unsigned char pad2[0x128 - 0x118];
    int f128;
    struct Sub sub;
    unsigned char pad3[0x140 - 0x134];
    struct Resp resp;
    unsigned char pad4[0x244 - 0x148];
    unsigned char flag;
};

int FUN_08867064(struct Obj *self)
{
    struct Ref *r;
    struct Ref *p_ref;
    struct Sub *sub;

    FUN_08866d9c(&r, self);
    sub = &self->sub;
    FUN_08866a7c(sub, &self->f114, &r, self->f128);
    p_ref = r;
    if (p_ref != 0) {
        int c;
        FUN_08ad3088(&DAT_08bef8f4);
        c = --p_ref->count;
        FUN_08ad2810(&DAT_08bef8f4);
        if (c == 0) {
            p_ref->vt[1](p_ref);
        }
    }
    FUN_0887c328(DAT_08bedba4, sub);
    if (sub->state == 6) {
        FUN_08866590(self);
        self->flag = 1;
    } else {
        struct Resp *resp = &self->resp;
        if (resp->a == 2 && resp->code == 404) {
            self->flag = 1;
        }
    }
    return 1;
}
// 15 differing words (65/64): entire body up to +0xb0 matches; at +0xd0 ours fills the `bne v1,v0` delay slot
// with `lw ra,36(sp)` from the target epilogue instead of `li v0,1` from the fall-through (adding 1 word).
// Ruled out: lang=c++ bool return (17), early returns (15), cc=gcc404xs-O2 (63).
// near miss from lane oc-lbp-20261121 (unreviewed)
