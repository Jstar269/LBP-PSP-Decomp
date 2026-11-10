/* LBPMATCH module=EBOOT.BIN start=0x08841c58 end=0x08841c9c symbol=FUN_08841c58 lang=c99 */
/* Mechanical translation of the disassembly (m2c c632946b, lbpm2c.py tag m3); names and types await human review. */

int FUN_088050b8(int);
int FUN_08a9e0cc(void *);
extern int DAT_08bae0d0;

void FUN_08841c58(void *arg0) {
    int temp_v1;

    temp_v1 = (*(int *)((char *)(arg0) + 8));
    (*(int **)((char *)(arg0) + 0)) = &DAT_08bae0d0;
    if (temp_v1 != 0) {
        FUN_088050b8(temp_v1);
    }
    FUN_08a9e0cc(arg0);
}
