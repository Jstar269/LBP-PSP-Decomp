/* LBPMATCH module=EBOOT.BIN start=0x088746c4 end=0x08874754 symbol=FUN_088746c4 lang=c99 */
/* Hand-written reconstruction from the disassembly (lane oc-lbp-20261121, Antigravity); matching idiom: volatile on b17 (keeps sb out of jal delay slot); exact match verified; names and types await human review. */

extern int DAT_08bef8f4;

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
    unsigned char b0c;
    unsigned char pad1[7];
    unsigned char b14;
    unsigned char pad2[2];
    volatile unsigned char b17;
};

void FUN_088746c4(struct Arg1Inner **param_1, struct Obj *param_2)
{
    int c;
    if ((*param_1)->flag != 0) {
        param_2->b0c = 1;
    }
    param_2->b14 = 1;
    param_2->b17 = 0;
    FUN_08ad3088(&DAT_08bef8f4);
    param_2->count2--;
    c = --param_2->count1;
    FUN_08ad2810(&DAT_08bef8f4);
    if (c == 0) {
        param_2->vt[1](param_2);
    }
    FUN_0888256c();
}
