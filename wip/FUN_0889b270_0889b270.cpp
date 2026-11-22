/* LBPMATCH module=EBOOT.BIN start=0x0889b270 end=0x0889b354 symbol=FUN_0889b270 lang=c++ */
/* Hand-written reconstruction from the disassembly (lane oc-lbp-20261121, Antigravity); exact match verified; names and types await human review. */

extern "C" void FUN_0889aa54(void *, int);

struct Item {
    void *vt;
    int state;
};

struct Vec {
    Item **begin;
    Item **end;
    inline Item *top()
    {
        if (begin == end || (end - begin) == 0) {
            return 0;
        }
        return *begin;
    }
};

extern "C" Vec *DAT_08bee224;

extern "C" void FUN_0889b270(bool param_1)
{
    if (DAT_08bee224->top() != 0) {
        if (DAT_08bee224->top()->state == 2) {
            FUN_0889aa54(DAT_08bee224->top(), 3);
        } else if (param_1) {
            FUN_0889aa54(DAT_08bee224->top(), 4);
        } else {
            FUN_0889aa54(DAT_08bee224->top(), 5);
        }
    }
}
// near miss from lane oc-lbp-20261121 (unreviewed)
