/* LBPMATCH module=EBOOT.BIN start=0x089569fc end=0x08956a20 symbol=FUN_089569fc lang=c99 */
/* Mechanical translation of the disassembly (m2c c632946b, lbpm2c.py tag m2); names and types await human review. */

extern int DAT_08957144;
extern int DAT_08957478;

void FUN_089569fc(int arg0, void **arg1) {
    (*(int (**)(void **, int *, int, int *))((char *)(*arg1) + 0x28))(arg1, &DAT_08957144, arg0, &DAT_08957478);
}
