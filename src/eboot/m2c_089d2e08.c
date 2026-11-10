/* LBPMATCH module=EBOOT.BIN start=0x089d2e08 end=0x089d2e3c symbol=FUN_089d2e08 lang=c99 */
/* Mechanical translation of the disassembly (m2c c632946b, lbpm2c.py tag m4); names and types await human review. */

void FUN_089d2e08(void *arg0) {
    int temp_v1;

    temp_v1 = (*(int *)((char *)(arg0) + 0x2C));
    switch (temp_v1) {                              /* irregular */
    case 0:
        (*(int *)((char *)(arg0) + 0x1C)) = (int) (*(short *)((char *)(arg0) + 0x10));
        return;
    case 1:
        (*(int *)((char *)(arg0) + 0x1C)) = (int) (*(short *)((char *)(arg0) + 0x12));
        return;
    }
}
