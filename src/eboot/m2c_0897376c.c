/* LBPMATCH module=EBOOT.BIN start=0x0897376c end=0x0897378c symbol=FUN_0897376c lang=c99 */
/* Mechanical translation of the disassembly (m2c c632946b, lbpm2c.py tag m2); names and types await human review. */

void FUN_0897376c(void *arg0) {
    void *temp_a1;
    void *temp_v1;

    temp_v1 = (*(void **)((char *)(arg0) + 0x18));
    temp_a1 = (*(void **)((char *)(temp_v1) + 0));
    (*(int *)((char *)(temp_v1) + 4)) = (int) ((*(int *)((char *)(temp_v1) + 4)) | 0x200);
    (*(int (**)(void *, void *))((char *)(temp_a1) + 0x24))(temp_v1, temp_a1);
}
