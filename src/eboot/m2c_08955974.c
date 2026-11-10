/* LBPMATCH module=EBOOT.BIN start=0x08955974 end=0x089559b8 symbol=FUN_08955974 lang=c99 */
/* Mechanical translation of the disassembly (m2c c632946b, lbpm2c.py tag m3); names and types await human review. */

int FUN_08950404(void *);
extern int DAT_08bb7ce8;

void FUN_08955974(void *arg0) {
    int (*temp_v1)(int);

    temp_v1 = (*(int (**)(int))((char *)(arg0) + 0x10));
    (*(int **)((char *)(arg0) + 0)) = &DAT_08bb7ce8;
    if (temp_v1 != 0) {
        temp_v1((*(int *)((char *)(arg0) + 4)));
        (*(int *)((char *)(arg0) + 4)) = 0;
    }
    FUN_08950404(arg0);
}
