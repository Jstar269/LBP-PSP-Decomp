/* LBPMATCH module=EBOOT.BIN start=0x0888cf30 end=0x0888cff0 symbol=FUN_0888cf30 lang=c99 */
/* Hand-written reconstruction from the disassembly (lane oc-lbp-20261121, Antigravity); exact match verified; names and types await human review. */

extern char DAT_08bafa48[];

struct Str {
    int pad[2];
    char str[1];
};

struct Attr {
    int pad[5];
    struct Str *name;
    struct Str *val;
};

extern void *FUN_08b9eb88(void *, int, unsigned int);
extern int FUN_08ba0d98(const char *, const char *);
extern struct Attr *FUN_08843824(struct Attr *);
extern void FUN_0888cddc(const char *, void *);

int FUN_0888cf30(const char *param_1, struct Attr *param_2, char *param_3, void *param_4)
{
    FUN_08b9eb88(param_3, 0, 20);
    while (*param_1 != 0) {
        *param_3++ = *param_1++;
    }
    while (param_2 != 0) {
        if (FUN_08ba0d98(param_2->name->str, DAT_08bafa48) == 0) {
            FUN_0888cddc(param_2->val->str, param_4);
            break;
        }
        param_2 = FUN_08843824(param_2);
    }
    return 1;
}
