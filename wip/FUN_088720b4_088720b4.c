/* LBPMATCH module=EBOOT.BIN start=0x088720b4 end=0x0887216c symbol=FUN_088720b4 lang=c99 */
/* Hand-written reconstruction from the disassembly (lane oc-lbp-20261121, Antigravity); near match; names and types await human review. */

extern int DAT_08bef8f4;

extern void FUN_08873454(void *);
extern void FUN_08a9df6c(void *);
extern void FUN_08ad3088(int *);
extern void FUN_08ad2810(int *);
extern void FUN_0888256c(void);

struct Arg1Inner {
    unsigned char pad[52];
    int flag;
};

struct Obj {
    void (**vt)(struct Obj *);
    int count1;
    int count2;
    unsigned char pad1[48 - 12];
    void *f30;
    unsigned char pad2[144 - 52];
    unsigned char flag;
    unsigned char pad3[3];
    void *f94;
};

void FUN_088720b4(struct Arg1Inner **param_1, struct Obj *param_2)
{
    int c;
    if ((*param_1)->flag == 0) {
        FUN_08873454(param_2->f30);
        param_2->flag = 1;
        FUN_08a9df6c(param_2->f94);
    } else {
        param_2->vt[4](param_2);
        FUN_08a9df6c(param_2->f94);
    }
    param_2->f94 = 0;
    FUN_08ad3088(&DAT_08bef8f4);
    c = --param_2->count1;
    param_2->count2--;
    FUN_08ad2810(&DAT_08bef8f4);
    if (c == 0) {
        param_2->vt[1](param_2);
    }
    FUN_0888256c();
}
// 3 differing words (46/46): only +0x30..+0x3c scheduling differs (ours hoists `lw a0,148(s2)` above `li v0,1; sb v0,144(s2)`,
// retail keeps `lw a0,148(s2)` in the `j` delay slot). Same in C++ with bool flag.
// near miss from lane oc-lbp-20261121 (unreviewed)
