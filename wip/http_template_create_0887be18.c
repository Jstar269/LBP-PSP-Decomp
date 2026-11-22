/* LBPMATCH module=EBOOT.BIN start=0x0887be18 end=0x0887bea8 symbol=http_template_create lang=c99 */
/* Hand-written reconstruction from the disassembly (lane oc-lbp-20261121, Antigravity); near match; names and types await human review. */

extern char DAT_08bb0870[];

extern void FUN_0887bdc8(void);
extern void FUN_0887bdd0(void);
extern void FUN_0887bde8(void);
extern void FUN_08baa4e4(void (*)(void), void (*)(void), void (*)(void));
extern int FUN_08baa48c(char *, int, int);
extern void FUN_08baa474(int, int);
extern void FUN_08baa484(int, int);
extern void FUN_08baa434(int, int);
extern void FUN_08baa42c(int);

struct Obj {
    unsigned char pad[448];
    unsigned char flag;
    unsigned char pad2[3];
    int tmpl;
};

void http_template_create(struct Obj *self)
{
    FUN_08baa4e4(FUN_0887bdc8, FUN_0887bdd0, FUN_0887bde8);
    self->tmpl = FUN_08baa48c(DAT_08bb0870, 1, 1);
    FUN_08baa474(self->tmpl, 30000000);
    FUN_08baa484(self->tmpl, 30000000);
    FUN_08baa434(self->tmpl, 30000000);
    FUN_08baa42c(self->tmpl);
    self->flag = 0;
}

// NEAR: 36/36 words; 5 differ at +0x48..+0x60.
// Remaining diff: instruction scheduling around jal 0x8baa474 and jal 0x8baa484
// (retail puts sw v0,452(s1) in jal 0x8baa474 delay slot and lw a0,452(s1) in
// jal 0x8baa484 delay slot; ours puts ori a1,s0,0xc380 in both delay slots).
// Ruled out: lang=c++, local int t = FUN_08baa48c(...), inline assignment inside
// FUN_08baa474(self->tmpl = FUN_08baa48c(...), 30000000) — all produce identical 5 diffs.
// near miss from lane oc-lbp-20261121 (unreviewed)
