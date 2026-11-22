/* LBPMATCH module=EBOOT.BIN start=0x0885e13c end=0x0885e1d4 symbol=FUN_0885e13c lang=c99 */
/* Hand-written reconstruction from the disassembly (lane oc-lbp-20261121, the OpenCode lane agent); names and types await human review. */

extern int DAT_08bef8f4;
void FUN_08ad3088(int *a, int *b);
void FUN_08ad2810(int *a);
void FUN_0888256c(void);

void FUN_0885e13c(int *param_1, int *param_2)
{
    if (*(int *)(param_1[0] + 52) != 0) {
        param_2[50] = 11;
    } else {
        *((unsigned char *)param_2 + 716) = 1;
    }
    FUN_08ad3088(&DAT_08bef8f4, param_2);
    int iVar1 = param_2[1];
    param_2[2]--;
    FUN_08ad2810(&DAT_08bef8f4);
    param_2[1] = iVar1 - 1;
    if (param_2[1] == 0) {
        void (*f)(int *) = *(void (**)(int *))((char *)param_2[0] + 4);
        f(param_2);
    }
    FUN_0888256c();
}
// Still differs: 19 words (first at +0x1c).
//   ours: beqz v0,0x885e16c; move s2,a1; li v0,11; j 0x885e174(delay: sw v0,200(a1)); 0x885e16c: li v0,1;
//         0x885e170: sb v0,716(a1); 0x885e174: lui s0,0x8bf; 0x885e178: move a1,s2; jal 0x8ad3088; tail
//         lw v0,8(s2); lw s1,4(s2); ...; jal 0x8ad2810; addiu s1,s1,-1; move a0,s2; bnez s1,0x885e1bc; sw s1,4(s2).
//   retail: bnez v0,0x885e170; move s2,a1; li v0,1; sb v0,716(a1); j 0x885e17c; 0x885e170: li v0,11;
//         0x885e174: sw v0,200(a1); 0x885e178: lui s1,0x8bf; jal 0x8ad3088; tail lw s0,4(s2); lw v0,8(s2); ...;
//         jal 0x8ad2810; sw s0,4(s2); bnez s0,0x885e1bc; move a0,s2.
// Ruled out: the virtual-call shape (lw v0,0(s2); lw v0,4(v0); jalr v0) matches; the difference is the head
// branch/j placement and register picks around the byte store and the iVar1-1 decrement.
// near miss from lane oc-lbp-20261121 (unreviewed)
