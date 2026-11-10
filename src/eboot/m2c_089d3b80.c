/* LBPMATCH module=EBOOT.BIN start=0x089d3b80 end=0x089d3bac symbol=FUN_089d3b80 lang=c99 */
/* Mechanical translation of the disassembly (m2c c632946b, lbpm2c.py tag m4); names and types await human review. */

void *FUN_089d3b80(int arg0, void **arg1) {
    void *var_a1;

    var_a1 = *arg1;
loop_1:
    if ((var_a1 != (arg0 + 0x90)) && ((*(unsigned short *)((char *)(var_a1) + 0x1E)) == 0)) {
        var_a1 = (*(void **)((char *)(var_a1) + 0));
        goto loop_1;
    }
    return var_a1;
}
