/* LBPMATCH module=EBOOT.BIN start=0x0885f314 end=0x0885f3ec symbol=FUN_0885f314 lang=c99 */
/* Hand-written reconstruction from the disassembly (lane oc-lbp-20261121, the OpenCode lane agent); names and types await human review. */

extern int DAT_08bef8f4;
extern int DAT_08be84ac;
void FUN_08ad3088(int *a, int b);
void FUN_08ad2810(int *a);
void FUN_08adbc24(int *a, int *b, int c);
void FUN_0885f314(int param_1, int param_2);

void FUN_0885f314(int param_1, int param_2)
{
    while (param_2 != 0) {
        int iVar1 = *(int *)(param_2 + 0xc);
        FUN_0885f314(param_1, iVar1);
        int *piVar3 = *(int **)(param_2 + 0x14);
        int uVar5 = *(int *)(param_2 + 8);
        if (piVar3 != 0) {
            FUN_08ad3088(&DAT_08bef8f4, iVar1);
            int iVar2 = piVar3[1] - 1;
            FUN_08ad2810(&DAT_08bef8f4);
            piVar3[1] = iVar2;
            if (iVar2 == 0) {
                void (*f)(int *) = *(void (**)(int *))((char *)piVar3[0] + 4);
                f(piVar3);
            }
        }
        int *piVar3t = (int *) DAT_08be84ac;
        int *piVar4 = piVar3t + 32;
        FUN_08ad3088(piVar4, param_2);
        FUN_08adbc24(piVar3t, param_2, 0x1c);
        FUN_08ad2810(piVar4);
        param_2 = uVar5;
    }
}
// Still differs (45 words; first at +0x24).
//   ours: j 0x885f3cc (loop check); lw s0,12(s2) (iVar1 in s0); lui s3,0x8bf; jal 0x885f314; move a1,s0;
//         lw s1,20(s2); ...; tail: lui v0,0x8bf; lw s0,-31572(v0); addiu s1,s0,128; jal 0x8ad3088; ...
//   retail: j 0x885f3c0 (loop check); lw a1,12(s2) (iVar1 in a1); jal 0x885f314; lui s3,0x8bf; lw s1,20(s2);
//         addiu a0,s3,-1804; beqz s1,0x885f390; lw s4,8(s2); jal 0x8ad3088; ... tail: addiu s1,s0,32.
// Ruled out: the virtual-call shape (lw v0,0(s1); lw v0,4(v0); jalr v0) matches; the head/tail scheduling,
//   iVar1's register, and the tail DAT+32/+128 difference remain.
// near miss from lane oc-lbp-20261121 (unreviewed)
