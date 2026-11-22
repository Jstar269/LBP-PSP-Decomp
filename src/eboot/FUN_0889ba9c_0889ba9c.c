/* LBPMATCH module=EBOOT.BIN start=0x0889ba9c end=0x0889bb80 symbol=FUN_0889ba9c lang=c99 */
/* Hand-written reconstruction from the disassembly (lane oc-lbp-20261121, Antigravity); exact match verified; names and types await human review. */

extern void *DAT_08bee224;
extern char DAT_08bb18a4[];
extern void *DAT_08bb17e8[];
extern char DAT_08bb18ac[];
extern void *DAT_08bb1838[];
extern char DAT_08bb18cc[];

extern void FUN_0889b800(void *, char *);
extern void *FUN_08a9e838(int);
extern void FUN_0889b0cc(void *);
extern void FUN_0889b014(void *, void *, char *);

struct Item {
    void **vt;
    int f4;
    int f8;
};

struct Obj {
    int f0;
    int f4;
    int f8;
    int f0c;
    int f10;
    int f14;
};

void FUN_0889ba9c(struct Obj *self, int param_2, int param_3, int param_4)
{
    if (param_2 == 1) {
        FUN_0889b800(DAT_08bee224, DAT_08bb18a4);
    }
    self->f8 = param_2;
    if (param_3 == 3) {
        void *mgr = DAT_08bee224;
        struct Item *p = (struct Item *)FUN_08a9e838(12);
        FUN_0889b0cc(p);
        p->vt = DAT_08bb17e8;
        FUN_0889b014(mgr, p, DAT_08bb18ac);
    } else {
        void *mgr = DAT_08bee224;
        struct Item *p = (struct Item *)FUN_08a9e838(12);
        FUN_0889b0cc(p);
        p->vt = DAT_08bb1838;
        FUN_0889b014(mgr, p, DAT_08bb18cc);
    }
    self->f14 = param_4;
}
