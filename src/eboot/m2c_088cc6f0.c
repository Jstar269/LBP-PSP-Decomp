/* LBPMATCH module=EBOOT.BIN start=0x088cc6f0 end=0x088cc70c symbol=FUN_088cc6f0 lang=c99 */
/* Mechanical translation of the disassembly (m2c c632946b, lbpm2c.py tag m2); names and types await human review. */

int FUN_08a2397c(int);

void FUN_088cc6f0(void *arg0) {
    void *temp_v0;

    temp_v0 = (*(void **)((char *)(arg0) + 0x1C));
    if (temp_v0 != 0) {
        FUN_08a2397c((*(int *)((char *)(temp_v0) + 0x2C)));
    }
}
