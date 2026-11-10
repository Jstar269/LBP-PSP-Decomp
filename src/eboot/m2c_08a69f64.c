/* LBPMATCH module=EBOOT.BIN start=0x08a69f64 end=0x08a69fa8 symbol=FUN_08a69f64 lang=c99 */
/* Mechanical translation of the disassembly (m2c c632946b, lbpm2c.py tag m3); names and types await human review. */

int FUN_08a50fbc();
int FUN_08a6ae78(void *);
int FUN_08a8e484(int, void *, int);
extern int DAT_08bc75c0;

void FUN_08a69f64(void *arg0) {
    (*(int **)((char *)(arg0) + 0)) = &DAT_08bc75c0;
    FUN_08a8e484(FUN_08a50fbc() + 0xB7C, arg0, (*(int *)((char *)(arg0) + 0x24C)));
    FUN_08a6ae78(arg0);
}
