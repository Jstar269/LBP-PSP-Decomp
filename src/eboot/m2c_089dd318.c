/* LBPMATCH module=EBOOT.BIN start=0x089dd318 end=0x089dd330 symbol=FUN_089dd318 lang=c99 */
/* Mechanical translation of the disassembly (m2c c632946b, lbpm2c.py tag m2); names and types await human review. */

void *FUN_089dd318(void *arg0) {
    void *var_v1;

    var_v1 = arg0;
    if ((*(unsigned char *)((char *)(arg0) + 0x4D)) == 0) {
        var_v1 = (*(void **)((char *)(arg0) + 0x18));
    }
    return var_v1;
}
