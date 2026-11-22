/* LBPMATCH module=EBOOT.BIN start=0x0886d8ec end=0x0886d99c symbol=FUN_0886d8ec lang=c99 */
/* Hand-written reconstruction from the disassembly (lane oc-lbp-20261121, Antigravity); exact match verified; names and types await human review. */

extern int DAT_08bef8f4;

extern void FUN_08873454(void *);
extern void FUN_08ad3088(int *);
extern void FUN_08ad2810(int *);
extern void FUN_0888256c(void);

struct Arg1Inner {
    unsigned char pad[64];
    int flag;
};

struct Obj {
    void (**vt)(struct Obj *);
    int count1;
    int count2;
    unsigned char pad[68 - 12];
    void *f44;
    void *f48;
};

void FUN_0886d8ec(struct Arg1Inner **param_1, struct Obj *param_2)
{
    int c;
    if ((*param_1)->flag == 0) {
        FUN_08873454(param_2->f44);
        FUN_08873454(param_2->f48);
        param_2->vt[5](param_2);
    } else {
        param_2->vt[4](param_2);
    }
    FUN_08ad3088(&DAT_08bef8f4);
    c = --param_2->count1;
    param_2->count2--;
    FUN_08ad2810(&DAT_08bef8f4);
    if (c == 0) {
        param_2->vt[1](param_2);
    }
    FUN_0888256c();
}
