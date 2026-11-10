/* LBPMATCH module=EBOOT.BIN start=0x08849dd4 end=0x08849e18 symbol=FUN_08849dd4 lang=c99 */
/* Mechanical translation of the disassembly (m2c c632946b, lbpm2c.py tag m3); names and types await human review. */

int FUN_0888f6b8(int);
int FUN_0888f6c0(int);

int FUN_08849dd4(void *arg0) {
    int temp_s1;
    unsigned char temp_s0;

    temp_s1 = arg0 + 0x110;
    FUN_0888f6b8(temp_s1);
    temp_s0 = (*(unsigned char *)((char *)(arg0) + 0xF4));
    FUN_0888f6c0(temp_s1);
    return temp_s0 ^ 1;
}
