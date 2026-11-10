/* LBPMATCH module=EBOOT.BIN start=0x08a76634 end=0x08a76674 symbol=FUN_08a76634 lang=c99 */
/* Mechanical translation of the disassembly (m2c c632946b, lbpm2c.py tag m3); names and types await human review. */

int FUN_08a9a0b8(void *);
int FUN_08a9e0cc(void *);
extern int DAT_08bc8178;

void FUN_08a76634(void *arg0) {
    (*(int **)((char *)(arg0) + 0)) = &DAT_08bc8178;
    FUN_08a9e0cc((*(void **)((char *)(arg0) + 0x1EC)));
    FUN_08a9a0b8(arg0);
    FUN_08a9e0cc(arg0);
}
