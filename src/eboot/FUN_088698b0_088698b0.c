/* LBPMATCH module=EBOOT.BIN start=0x088698b0 end=0x08869960 symbol=FUN_088698b0 lang=c99 */
/* Hand-written reconstruction from the disassembly (lane oc-lbp-20261121, Antigravity); exact match verified; names and types await human review. */

extern int DAT_08bef8f4;

extern void *FUN_08a9e838(int);
extern void FUN_08876bd8(void *, void *);
extern void FUN_088697d0(void *, void *, void *);
extern void FUN_08ad3088(int *);
extern void FUN_08ad2810(int *);

struct Ref {
    void (**vt)(struct Ref *);
    int count;
};

void FUN_088698b0(void *self, char **param_2)
{
    struct Ref *r;
    struct Ref *p;
    struct Ref *p_ref;

    p = FUN_08a9e838(24);
    FUN_08876bd8(p, param_2);
    r = p;
    FUN_088697d0(self, &r, *param_2 + 16);
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
}
