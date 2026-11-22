/* LBPMATCH module=EBOOT.BIN start=0x08860e74 end=0x08860f20 symbol=FUN_08860e74 lang=c99 */
/* Hand-written reconstruction from the disassembly (lane oc-lbp-20261121, the OpenCode lane agent); exact match verified; names and types await human review. */
// wip: 3 words differ. Retail prologue saves: sw s0,48(sp); sw ra,60(sp); sw s2,56(sp); sw s1,52(sp);
// ours: sw s0,48(sp); sw ra,56(sp); sw s1,52(sp) - ours does not save the register holding the
// 0x88451b8 result (retail keeps it in s2, ours assigns the result to extraout_v0 which GCC places in s1).
// Retail tail: move a0,s2 / move v0,s2; ours: move a0,s1 / move v0,s1.
// Also: retail does addiu s1,s0,32; move a0,s1; move a1,sp (s1 = extraout_v0_00+8); ours:
// addiu a0,s0,32; move a1,sp (no intermediate s1).
// Everything else matches, including the store *extraout_v0_00 = (int)&PTR_FUN_08bae290 and
// FUN_08ba0e8c result in a2 via delay slot.

extern char DAT_08baf500[];
extern char PTR_FUN_08bae290[];

extern int *FUN_08a9e838(int arg);
extern int *FUN_088451b8(int arg, char *name);
extern void FUN_08878ab8(int arg, int dst);
extern int *FUN_08843f04(int arg, int size);
extern unsigned int FUN_08ba0e8c(int arg);
extern void FUN_08842ef4(int dst, int src, int val);
extern void FUN_08844248(int a, int b);

int *FUN_08860e74(unsigned int *param_1)
{
    int *extraout_v0;
    int *extraout_v0_00;
    unsigned int uVar1;
    char acStack_40[48];

    extraout_v0 = FUN_08a9e838(0x50);
    extraout_v0 = FUN_088451b8(extraout_v0, DAT_08baf500);
    FUN_08878ab8(*param_1, (int)acStack_40);
    extraout_v0_00 = FUN_08a9e838(0x30);
    extraout_v0_00 = FUN_08843f04(extraout_v0_00, 4);
    *extraout_v0_00 = (int)&PTR_FUN_08bae290;
    uVar1 = FUN_08ba0e8c((int)acStack_40);
    FUN_08842ef4((int *)(extraout_v0_00 + 8), acStack_40, uVar1);
    *(unsigned char *)(extraout_v0_00 + 0xb) = 0;
    FUN_08844248(extraout_v0, extraout_v0_00);
    return extraout_v0;
}
// near miss from lane oc-lbp-20261121 (unreviewed)
