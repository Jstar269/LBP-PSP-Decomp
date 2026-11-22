/* LBPMATCH module=EBOOT.BIN start=0x0885df50 end=0x0885e004 symbol=FUN_0885df50 rodata=0x08bafa04 lang=c99 */
/* Hand-written reconstruction from the disassembly (lane oc-lbp-20261121, the OpenCode lane agent); exact match verified; names and types await human review. */

extern float DAT_08bab59c;
extern float DAT_08babcb4;
extern float DAT_08bab5d8;
extern float DAT_08bab5fc;
extern float DAT_08bab5e8;
extern float DAT_08baf9f8;
extern float DAT_08baf9fc;
extern float DAT_08bab5c8;
extern float DAT_08babd1c;
extern float DAT_08bab5a0;
extern float DAT_08bafa00;

float FUN_0885df50(int param_1)
{
    switch (*(unsigned *)(param_1 + 200)) {
    case 10:
    case 11:
        return DAT_08bab59c;
    case 0:
        return DAT_08babcb4;
    case 1:
        return DAT_08bab5d8;
    case 2:
        return DAT_08bab5fc;
    case 3:
        return DAT_08bab5e8;
    case 4:
        return DAT_08baf9f8;
    case 5:
        return DAT_08baf9fc;
    case 6:
        return DAT_08bab5c8;
    case 7:
        return DAT_08babd1c;
    case 8:
        return DAT_08bab5a0;
    case 9:
        return DAT_08bafa00;
    }
    return 0.0f;
}
