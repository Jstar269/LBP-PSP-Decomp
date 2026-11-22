/* LBPMATCH module=EBOOT.BIN start=0x08883308 end=0x088833d8 symbol=FUN_08883308 lang=c99 */
/* Hand-written reconstruction from the disassembly (lane oc-lbp-20261121, Antigravity); exact match verified; names and types await human review. */

struct Obj {
    int rc;
};

extern struct Obj *DAT_08bedfb0;

extern struct Obj *FUN_08a9e838(int);
extern void FUN_088817a0(struct Obj *);
extern void FUN_0888091c(struct Obj *);
extern void FUN_08adb8a4(struct Obj *);

struct Obj **FUN_08883308(struct Obj **out)
{
    struct Obj *p = FUN_08a9e838(2596);
    struct Obj *old;
    struct Obj *cur;

    FUN_088817a0(p);
    if (p != 0) {
        p->rc++;
    }
    old = DAT_08bedfb0;
    DAT_08bedfb0 = p;
    if (old != 0 && --old->rc == 0) {
        FUN_0888091c(old);
        FUN_08adb8a4(old);
    }
    if (p != 0 && --p->rc == 0) {
        FUN_0888091c(p);
        FUN_08adb8a4(p);
    }
    cur = DAT_08bedfb0;
    *out = cur;
    if (cur != 0) {
        cur->rc++;
    }
    return out;
}
