/* LBPMATCH module=EBOOT.BIN start=0x08a783ec end=0x08a78430 symbol=FUN_08a783ec lang=c99 */
/* Mechanical translation of the disassembly (m2c c632946b, lbpm2c.py tag m3); names and types await human review. */

int FUN_08904cd4(void *);
int FUN_0890d894(void *);
int FUN_08a1d7d0(int);
int FUN_08a710e4();

void FUN_08a783ec(void *arg0) {
    int temp_v0;

    FUN_08a710e4();
    temp_v0 = (*(int *)((char *)(arg0) + 0x188));
    if (temp_v0 != 0) {
        FUN_08a1d7d0(temp_v0);
    }
    FUN_08904cd4(arg0 + 0x64B0);
    FUN_0890d894(arg0 + 0x394);
}
