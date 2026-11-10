/* LBPMATCH module=EBOOT.BIN start=0x0890c3dc end=0x0890c414 symbol=FUN_0890c3dc lang=c99 */
/* Mechanical translation of the disassembly (m2c c632946b, lbpm2c.py tag m4); names and types await human review. */

int FUN_08910574(int, int);
extern float DAT_08bef16c;

void FUN_0890c3dc(void *arg0) {
    if (!((*(float *)((char *)(arg0) + 0)) <= DAT_08bef16c)) {
        (*(signed char *)((char *)(arg0) + 0x28)) = 1;
        FUN_08910574((*(int *)((char *)((*(void **)((char *)(arg0) + 0x24))) + 0x660)), 3);
    }
}
