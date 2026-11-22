/* LBPMATCH module=EBOOT.BIN start=0x0888a644 end=0x0888a72c symbol=FUN_0888a644 lang=c99 */
/* Hand-written reconstruction from the disassembly (lane oc-lbp-20261121, Antigravity); near match; names and types await human review. */

struct Obj {
    int f0;
    int f4;
    unsigned char pad1[92 - 8];
    unsigned char b5c;
    unsigned char pad2[1236 - 93];
    int f4d4;
    unsigned char pad3[2];
    unsigned char buf[0x10000];
};

extern struct Obj *DAT_08bedd40;
extern int DAT_08bee0e4;

extern void FUN_08baa0ac(void);
extern void *FUN_08b9eb88(void *, int, unsigned int);
extern void FUN_08baa6b4(void *, void *, int);
extern void FUN_08baa6ac(int *, int *);
extern int FUN_088876c0(int);
extern void FUN_08baa974(void);

int FUN_0888a644(void *param_1, int param_2)
{
    if (param_2 >= 0) {
        int a, b;
        DAT_08bedd40->b5c = 0;
        FUN_08baa0ac();
        DAT_08bee0e4 = param_2;
        FUN_08b9eb88(DAT_08bedd40->buf, 0, 0x10000);
        FUN_08baa6b4(param_1, DAT_08bedd40->buf, DAT_08bee0e4);
        a = 1;
        b = 0;
        FUN_08baa6ac(&a, &b);
        if (a == 0 || FUN_088876c0(b) != 0) {
            DAT_08bedd40->f4 = 5;
        } else {
            DAT_08bedd40->f4d4 = 6;
            DAT_08bedd40->f4 = 0;
        }
    } else {
        DAT_08bedd40->f4d4 = 7;
        DAT_08bedd40->f4 = 0;
        FUN_08baa974();
    }
    return 0;
}

// NEAR: 58/58 words; 1 differ at +0x80.
// Remaining diff: beqz v0,0x888a6dc vs retail beqzl v0,0x888a6dc (branch-likely prediction hint).
// Ruled out: inverting condition with unsigned char FUN_088876c0 (60/58 words, 27 differ), lang=c++ bool (63/58 words, 62 differ).
// near miss from lane oc-lbp-20261121 (unreviewed)
