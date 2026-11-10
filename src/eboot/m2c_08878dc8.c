/* LBPMATCH module=EBOOT.BIN start=0x08878dc8 end=0x08878e08 symbol=FUN_08878dc8 lang=c99 */
/* Mechanical translation of the disassembly (m2c c632946b, lbpm2c.py tag m3); names and types await human review. */

int FUN_08882614(int, int);
int FUN_088917a0(void *);

void FUN_08878dc8(void *arg0) {
    int temp_v1;

    temp_v1 = (*(int *)((char *)(arg0) + 0xC4));
    if (temp_v1 != 3) {
        FUN_08882614(0, temp_v1);
    }
    FUN_088917a0(arg0 + 0xC);
}
