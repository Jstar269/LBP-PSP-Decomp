/* LBPMATCH module=EBOOT.BIN start=0x088a1170 end=0x088a1210 symbol=FUN_088a1170 lang=c99 */
/* Hand-written reconstruction from the disassembly (lane oc-lbp-20261121, Antigravity); exact match verified; names and types await human review. */

extern int DAT_08bee3b4;
extern char DAT_08bb2668[];

extern void FUN_08aee0ec(void *, void *);
extern void FUN_08baa50c(void *);
extern void FUN_08baa524(void *, char *);
extern int FUN_08adbd60(char *);
extern void FUN_08aee3a8(void *);
extern void FUN_0889be7c(void *, int, char *, void *);
extern void FUN_0889c064(void *);

struct Params {
    int f0;
    int pad1[5];
    int f18;
    int pad2[6];
    int f34;
    unsigned char pad3[312 - 56];
};

void FUN_088a1170(void *param_1)
{
    unsigned char mac[6];
    char str[34];
    struct Params p;

    FUN_08aee0ec(&p, 0);
    FUN_08baa50c(mac);
    FUN_08baa524(mac, str);
    p.f0 = 4;
    p.f18 = DAT_08bee3b4;
    p.f34 = FUN_08adbd60(str);
    FUN_08aee3a8(&p);
    FUN_0889be7c(param_1, 0, DAT_08bb2668, 0);
    FUN_0889c064(param_1);
}
