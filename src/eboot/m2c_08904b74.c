/* LBPMATCH module=EBOOT.BIN start=0x08904b74 end=0x08904bb4 symbol=FUN_08904b74 lang=c99 */
/* Mechanical translation of the disassembly (m2c c632946b, lbpm2c.py tag m3); names and types await human review. */

int FUN_08841310(int, void *);
extern int DAT_08bedb50;

void FUN_08904b74(void *arg0) {
    if (((*(unsigned char *)((char *)(arg0) + 0x1E0)) != 0) && (FUN_08841310(DAT_08bedb50, arg0) != 0)) {
        (*(unsigned char *)((char *)(arg0) + 0x1E0)) = 0U;
    }
}
