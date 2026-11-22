/* LBPMATCH module=EBOOT.BIN start=0x0885f500 end=0x0885f5b4 symbol=FUN_0885f500 lang=c99 */
/* Hand-written reconstruction from the disassembly (lane oc-lbp-20261121, the OpenCode lane agent); names and types await human review. */

extern int DAT_08bef8f4;
extern int DAT_08be84ac;
void FUN_08ad3088(int *a, int *b);
int FUN_08adbb84(int *a, int b);
void FUN_08ad2810(int *a);
void FUN_0884d5f4(int *a, int *b);

int FUN_0885f500(int param_1, int *param_2)
{
    int *piVar1 = (int *) DAT_08be84ac;
    int *piVar5 = piVar1 + 8;
    FUN_08ad3088(piVar5, param_2);
    int iVar2 = FUN_08adbb84(piVar1, 0x20);
    int *p = (int *)(iVar2 + 0x10);
    FUN_08ad2810(piVar5);
    if (p != 0) {
        FUN_0884d5f4(p, param_2);
        int iVar4 = param_2[3];
        p[3] = iVar4;
        if (iVar4 != 0) {
            FUN_08ad3088(&DAT_08bef8f4, param_2);
            *(int *)(iVar4 + 4) = *(int *)(iVar4 + 4) + 1;
            FUN_08ad2810(&DAT_08bef8f4);
        }
    }
    return iVar2;
}
// Still differs (23 words; first at +0x50): head branch/move scheduling — ours sets a0=p before the branch
// and moves a1=param_2 inside the then (0885f550 move a0,s1; 0885f554 bezv s1,0x885f594; 0885f558 move a1,s2);
// retail sets a1=param_2 before the branch and a0=p in the then (0885f550 move a1,s2; 0885f554 bezv s1,0x885f590;
// 0885f558 move a0,s1). Ours is 1 word longer overall (46/45). Tail differences after that are the same words
// shifted: ours 0885f568 move a1,s2; 0885f56c addiu a0,s3,-1804; 0885f570 bezv s0,0x885f594; 0885f574 sw s0,12(s1);
// 0885f578 jal 0x8ad3088; 0885f57c nop; 0885f580 lw v0,4(s0); 0885f584 addiu a0,s3,-1804; 0885f588 addiu v0,v0,1;
// 0885f58c jal 0x8ad2810; 0885f590 sw v0,4(s0); 0885f594 move v0,s4 — retail: 0885f568 addiu a0,s3,-1804;
// 0885f56c bezv s0,0x885f590; 0885f570 sw s0,12(s1); 0885f574 jal 0x8ad3088; 0885f578 nop; 0885f57c lw v0,4(s0);
// 0885f580 addiu a0,s3,-1804; 0885f584 addiu v0,v0,1; 0885f588 jal 0x8ad2810; 0885f58c sw v0,4(s0);
// 0885f590 move v0,s4.
// Ruled out: the call/guard/store shapes match retail's (jal 0x884d5f4 with a0=p, a1=param_2; store at p+12;
// jal 0x8ad3088/0x8ad2810 with a0=&DAT_08bef8f4); the difference is branch/move order and the extra word.
// near miss from lane oc-lbp-20261121 (unreviewed)
