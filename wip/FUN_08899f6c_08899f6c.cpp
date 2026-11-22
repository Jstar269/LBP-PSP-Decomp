/* LBPMATCH module=EBOOT.BIN start=0x08899f6c end=0x0889a028 symbol=FUN_08899f6c lang=c++ rodata=0x08bb14e8 */
/* Hand-written reconstruction from the disassembly (lane oc-lbp-20261121, Antigravity); exact match verified; names and types await human review. */

extern "C" void FUN_08baa4fc(int *, int *);
extern "C" void FUN_08aedb6c(int *);

extern "C" void FUN_08899f6c(void)
{
    int a;
    int b;
    int c = 1;
    int val = 2;

    FUN_08baa4fc(&c, &b);
    switch (c) {
    case 0: val = 3; break;
    case 2: val = 8; break;
    case 3: val = 6; break;
    case 4: val = 7; break;
    case 5: val = 5; break;
    case 6: val = 9; break;
    case 7: val = 10; break;
    case 8: val = 17; break;
    case 10: val = 12; break;
    case 11: val = 11; break;
    case 9: val = 4; break;
    case 1: val = 2; break;
    }
    a = val;
    FUN_08aedb6c(&a);
}
// near miss from lane oc-lbp-20261121 (unreviewed)
