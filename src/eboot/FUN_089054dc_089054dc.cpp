/* LBPMATCH module=EBOOT.BIN start=0x089054dc end=0x0890556c symbol=FUN_089054dc lang=c++ */
/* Hand-written reconstruction from the disassembly (lane oc-lbp-20261121, Antigravity); exact match verified; names and types await human review. */

struct Alloc {
    int start;
    int cur;
    int end;
};

struct Entry {
    int pad[6];
    void *f18;
};

extern "C" Alloc *DAT_08bdc188;
extern "C" void FUN_08904c6c(Entry *);
extern "C" void FUN_08a26bec(void **, void *, void *, int);

extern "C" void FUN_089054dc(Entry *param_1)
{
    FUN_08904c6c(param_1);
    Alloc *a = DAT_08bdc188;
    if (a->end - (a->cur != 0 ? a->cur - a->start : 0) > 0x6000) {
        for (int i = 0; i < 12; i++) {
            if (param_1[i].f18 != 0) {
                FUN_08a26bec(&param_1[i].f18, 0, 0, 0);
            }
        }
    }
}
