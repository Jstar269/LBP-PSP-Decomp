/* LBPMATCH module=EBOOT.BIN start=0x088c6b30 end=0x088c6bd0 symbol=FUN_088c6b30 lang=c99 */
/* Hand-written reconstruction from the disassembly (lane oc-lbp-20261121, Antigravity); exact match verified; names and types await human review. */

struct Obj1 {
    int (**vt)(struct Obj1 *);
};

struct Obj2 {
    void (**vt)(struct Obj2 *, int);
};

extern void *DAT_08bedb1c;
extern void FUN_089463d4(void *);
extern void *FUN_08a9e838(int);
extern void FUN_088bf55c(struct Obj1 *);
extern void FUN_088c2380(struct Obj2 *, struct Obj1 *, int);
extern void FUN_0883bd5c(void *, struct Obj2 *);
extern void FUN_088b2aa4(void *);
extern void FUN_08804f3c(void *);

void FUN_088c6b30(void)
{
    struct Obj1 *a;
    struct Obj2 *b;
    int sp[4];

    FUN_089463d4(DAT_08bedb1c);
    a = (struct Obj1 *)FUN_08a9e838(52);
    FUN_088bf55c(a);
    b = (struct Obj2 *)FUN_08a9e838(148);
    FUN_088c2380(b, a, 9);
    b->vt[19](b, a->vt[16](a));
    FUN_0883bd5c(sp, b);
    FUN_088b2aa4(sp);
    FUN_08804f3c(sp);
}
