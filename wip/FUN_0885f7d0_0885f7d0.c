/* LBPMATCH module=EBOOT.BIN start=0x0885f7d0 end=0x0885f8b4 symbol=FUN_0885f7d0 lang=c99 */
/* Hand-written reconstruction from the disassembly (lane oc-lbp-20261121, the OpenCode lane agent); exact match verified; names and types await human review. */
// wip: prologue scheduling differs (57 words). Retail: `addiu v0,a0,4` first, then `move s4,a2` (param_3),
// saves, `bnez a1` early; ours: `move v1,a1` early, DAT load + `addiu s4,v0,32` early, branch `bnez v1` later;
// param_3 in s2 / param_4 in s1 vs retail s4 / a3. Semantics match; only schedule.
// Ruled out: piVar2 = DAT+8 (struct Pool size 33 -> +264); if/else-if vs boolean cVar3 forms;
// piVar2 = (char*)DAT+32.

extern struct Pool *DAT_08be84ac;

struct Pool {
  char pad[32];
  char lock[1];
};

int FUN_0885f7d0(int param_1, int param_2, int param_3, unsigned int *param_4)
{
    int *piVar1;
    int *piVar2;
    int cVar3;

    piVar1 = (int *)DAT_08be84ac;
    cVar3 = ((param_2 != 0) || (param_3 == param_1 + 4) ||
             (*param_4 < *(unsigned int *)(param_3 + 16)));
    piVar2 = (int *)((char *)DAT_08be84ac + 32);
    FUN_08ad3088(piVar2, param_2, param_3, param_4);
    piVar1 = (int *)FUN_08adbb84(DAT_08be84ac, 28);
    FUN_08ad2810(piVar2);
    if (piVar1 != (int *)0xfffffff0) {
        piVar1[4] = *param_4;
        FUN_0885e9dc(piVar1 + 5, (int *)param_4 + 1);
    }
    FUN_08b899e4(cVar3, piVar1, param_3, param_1 + 4);
    *(int *)(param_1 + 20) = *(int *)(param_1 + 20) + 1;
    return (int)piVar1;
}
// near miss from lane oc-lbp-20261121 (unreviewed)
