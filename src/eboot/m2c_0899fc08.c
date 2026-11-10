/* LBPMATCH module=EBOOT.BIN start=0x0899fc08 end=0x0899fc4c symbol=FUN_0899fc08 lang=c99 */
/* Mechanical translation of the disassembly (m2c c632946b, lbpm2c.py tag m3); names and types await human review. */

int FUN_0896cba8();
extern int DAT_08bc0538;

void FUN_0899fc08(void *arg0, int arg1) {
    FUN_0896cba8();
    (*(int **)((char *)(arg0) + 0)) = &DAT_08bc0538;
    (*(int *)((char *)(arg0) + 0x68)) = arg1;
    (*(int *)((char *)(arg0) + 0x6C)) = 0;
}
