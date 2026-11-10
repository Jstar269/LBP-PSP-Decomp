/* LBPMATCH module=EBOOT.BIN start=0x088c5f94 end=0x088c5fd8 symbol=FUN_088c5f94 lang=c99 */
/* Mechanical translation of the disassembly (m2c c632946b, lbpm2c.py tag m3); names and types await human review. */

int FUN_08ad2810(int);
int FUN_08ad3088(int);

void FUN_088c5f94(void *arg0) {
    int temp_s1;

    temp_s1 = arg0 + 0x14;
    FUN_08ad3088(temp_s1);
    (*(int *)((char *)(arg0) + 8)) = (int) ((*(int *)((char *)(arg0) + 8)) + 1);
    FUN_08ad2810(temp_s1);
}
