/* LBPMATCH module=EBOOT.BIN start=0x088a05cc end=0x088a0604 symbol=FUN_088a05cc lang=c99 */
/* Mechanical translation of the disassembly (m2c c632946b, lbpm2c.py tag m4); names and types await human review. */

int FUN_0889a8d0(int *, int, int);
extern int DAT_08bb1f50;
extern void **DAT_08bee384;

void FUN_088a05cc(int arg0, int arg1) {
    void *temp_v1;

    if (DAT_08bee384 != 0) {
        temp_v1 = *DAT_08bee384;
        if (temp_v1 != 0) {
            (*(signed char *)((char *)(temp_v1) + 0x15)) = 1;
        }
    }
    FUN_0889a8d0(&DAT_08bb1f50, arg0, arg1);
}
