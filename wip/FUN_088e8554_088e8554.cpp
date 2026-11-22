/* LBPMATCH module=EBOOT.BIN start=0x088e8554 end=0x088e85e4 symbol=FUN_088e8554 lang=c++ */
/* Hand-written reconstruction from the disassembly (lane oc-lbp-20261121, Antigravity); near match; names and types await human review. */

struct Page {
    int f0;
    int kind;
};

struct Vec {
    int pad[2];
    Page **begin;
    Page **end;

    inline Page *top()
    {
        return (begin == end) ? 0 : end[-1];
    }
};

struct Ui {
    int pad1[5];
    Vec vec;
    unsigned char pad2[0xa0 - 0x24];
    int fa0;
};

extern "C" void *DAT_08beed14;
extern "C" int FUN_088db95c(void *);
extern "C" void FUN_088f7950(Page *, int, unsigned int);

extern "C" void FUN_088e8554(Ui *param_1)
{
    if (FUN_088db95c(DAT_08beed14) != 0) {
        while (param_1->vec.top()->kind != 6) {
            FUN_088f7950(param_1->vec.top(), 1, 0xffff);
        }
    }
    if (param_1->fa0 != 0) {
        param_1->fa0 = 0;
    }
}

// Near-miss note: Branch/regalloc diff on 34/36 words.
// near miss from lane oc-lbp-20261121 (unreviewed)
