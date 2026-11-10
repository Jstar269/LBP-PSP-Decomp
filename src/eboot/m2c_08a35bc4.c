/* LBPMATCH module=EBOOT.BIN start=0x08a35bc4 end=0x08a35c04 symbol=FUN_08a35bc4 lang=c99 */
/* Mechanical translation of the disassembly (m2c c632946b, lbpm2c.py tag m3); names and types await human review. */

int FUN_08a356a4();
int FUN_08a5582c(void *);
extern int DAT_08bc42c8;
extern int DAT_08bc441c;

void FUN_08a35bc4(void *arg0) {
    FUN_08a356a4();
    (*(int **)((char *)(arg0) + 0)) = &DAT_08bc42c8;
    (*(int **)((char *)(arg0) + 0x24C)) = &DAT_08bc441c;
    FUN_08a5582c(arg0 + 0x250);
}
