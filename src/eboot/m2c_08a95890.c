/* LBPMATCH module=EBOOT.BIN start=0x08a95890 end=0x08a958d4 symbol=FUN_08a95890 lang=c99 */
/* Mechanical translation of the disassembly (m2c c632946b, lbpm2c.py tag m3); names and types await human review. */

int FUN_08a0a8ec(void *);
int FUN_08a9e0cc(int);
extern int DAT_08bc3148;
extern int DAT_08bca3a8;

void FUN_08a95890(void *arg0) {
    (*(int **)((char *)(arg0) + 0)) = &DAT_08bca3a8;
    FUN_08a9e0cc((*(int *)((char *)(arg0) + 0xF8)));
    (*(int **)((char *)(arg0) + 0)) = &DAT_08bc3148;
    FUN_08a0a8ec(arg0);
}
