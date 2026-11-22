/* LBPMATCH module=EBOOT.BIN start=0x08877f30 end=0x08877fe4 symbol=FUN_08877f30 lang=c99 */
/* Hand-written reconstruction from the disassembly (lane oc-lbp-20261121, Antigravity); exact match verified; names and types await human review. */

extern int DAT_08bef8f4;

extern void FUN_0886cb2c(void *, void *, int, int);
extern void FUN_08a9e0c4(void *);
extern void FUN_08ad3088(int *);
extern void FUN_08ad2810(int *);
extern void FUN_0888256c(void);

struct Obj {
    void (**vt)(struct Obj *);
    int count1;
    int count2;
    unsigned char pad[60 - 12];
    void *f3c;
    int f40;
    void *f44;
};

void FUN_08877f30(void *param_1, struct Obj *param_2)
{
    int c;
    FUN_0886cb2c(param_2, param_2->f3c, param_2->f40, 1);
    param_2->f3c = 0;
    param_2->vt[5](param_2);
    if (param_2->f44 != 0) {
        FUN_08a9e0c4(param_2->f44);
    }
    param_2->f44 = 0;
    FUN_08ad3088(&DAT_08bef8f4);
    c = --param_2->count1;
    param_2->count2--;
    FUN_08ad2810(&DAT_08bef8f4);
    if (c == 0) {
        param_2->vt[1](param_2);
    }
    FUN_0888256c();
}
