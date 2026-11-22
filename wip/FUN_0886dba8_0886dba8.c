/* LBPMATCH module=EBOOT.BIN start=0x0886dba8 end=0x0886dc68 symbol=FUN_0886dba8 lang=c99 */
/* Hand-written reconstruction from the disassembly (lane oc-lbp-20261121, Antigravity); near match; names and types await human review. */

extern int DAT_08bef8f4;

extern void *FUN_08a9e838(int);
extern void FUN_0887367c(void *, void *);
extern void FUN_0886daf8(void *, void *);
extern void FUN_08ad3088(int *);
extern void FUN_08ad2810(int *);

struct Ref {
    void (**vt)(struct Ref *);
    int count;
};

struct Obj {
    unsigned char pad[68];
    struct Ref *ref;
};

void FUN_0886dba8(void *param_1, void *param_2, struct Obj *param_3)
{
    struct Ref *r;
    struct Ref *p;
    struct Ref *p_ref;

    p = FUN_08a9e838(68);
    FUN_0887367c(p, param_2);
    r = p;
    FUN_0886daf8(&param_3->ref, &r);
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
    param_3->ref->vt[7](param_3->ref);
}
// 41 differing words (47/48): every instruction matches except retail does `move s2,a2; addiu s2,s2,68` in the prologue;
// with a local `slot = &param_3->ref` ours folds to `addiu s2,a2,68` (42 differ), without it ours keeps `s2 = a2` and adds 68 later (41 differ).
// near miss from lane oc-lbp-20261121 (unreviewed)
