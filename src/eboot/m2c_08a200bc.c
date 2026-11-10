/* LBPMATCH module=EBOOT.BIN start=0x08a200bc end=0x08a20100 symbol=FUN_08a200bc lang=c99 */
/* Mechanical translation of the disassembly (m2c c632946b, lbpm2c.py tag m3); names and types await human review. */

int FUN_08a1ffac(void *, unsigned char);
int FUN_08a24424(int, int);
int FUN_08a25d40(void *);
extern int DAT_08bdc188;

void FUN_08a200bc(void *arg0) {
    FUN_08a24424((*(int *)((char *)((*(void **)((char *)(arg0) + 0))) + 0xA8)), DAT_08bdc188);
    FUN_08a25d40(arg0 + 0x10);
    FUN_08a1ffac(arg0 + 0xDC, (*(unsigned char *)((char *)(arg0) + 0xE0)));
}
