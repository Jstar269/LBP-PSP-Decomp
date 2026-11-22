/* LBPMATCH module=EBOOT.BIN start=0x08868614 end=0x088686bc symbol=FUN_08868614 lang=c99 */
/* Hand-written reconstruction from the disassembly (lane oc-lbp-20261121, Antigravity); exact match verified; names and types await human review. */

extern char DAT_08baf004[];
extern char DAT_08baeb74[];
extern char DAT_08bd6488[];

extern void FUN_088432e8(void *);
extern void *FUN_08a9e838(int);
extern void FUN_088457bc(void *, char *, char *, char *);
extern void FUN_08844248(void *, void *);
extern void FUN_088684a0(void *);
extern long long FUN_08878d74(void *);

struct Obj {
    unsigned char pad1[432];
    int state;
    unsigned char pad2[668 - 436];
    int sub;
    unsigned char pad3[748 - 672];
    int flag;
};

int FUN_08868614(struct Obj *self)
{
    void *sub = &self->sub;
    void *p;

    self->state = 0;
    FUN_088432e8(sub);
    p = FUN_08a9e838(56);
    FUN_088457bc(p, DAT_08baf004, DAT_08baeb74, DAT_08bd6488);
    FUN_08844248(sub, p);
    if (self->flag != 0) {
        FUN_088684a0(self);
        return (int)FUN_08878d74(self);
    }
    self->state = 3;
    return 0;
}
