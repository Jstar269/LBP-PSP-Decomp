/* LBPMATCH module=EBOOT.BIN start=0x0889ce84 end=0x0889cf24 symbol=FUN_0889ce84 lang=c99 */
/* Hand-written reconstruction from the disassembly (lane oc-lbp-20261121, Antigravity); exact match verified; names and types await human review. */

extern void *FUN_08aee084(void);
extern void FUN_08b2f318(void *);
extern void FUN_08b31d10(void *, void *);
extern void FUN_08b2f364(void *, void *);

struct S1 {
    int f0;
    int pad[11];
};

struct S2 {
    void *f0;
    unsigned char f4;
    unsigned char pad1[3];
    int f8;
    int f0c[11];
    int f38;
    int pad2[3];
    int f48;
    int pad3;
};

void FUN_0889ce84(int param_1, int param_2, int param_3, unsigned char param_4)
{
    struct S1 s1;
    struct S2 s2;
    void *p;

    p = FUN_08aee084();
    FUN_08b2f318(&s2);
    s2.f0 = p;
    s2.f4 = param_4;
    s2.f8 = param_3;
    s2.f38 = param_2;
    s2.f48 = param_1;
    FUN_08b31d10(p, s2.f0c);
    s2.f0c[0] = -1;
    s1.f0 = 0;
    FUN_08b2f364(&s2, &s1);
}
