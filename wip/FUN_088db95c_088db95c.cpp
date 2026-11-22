/* LBPMATCH module=EBOOT.BIN start=0x088db95c end=0x088dba48 symbol=FUN_088db95c lang=c++ */
/* Hand-written reconstruction from the disassembly (lane oc-lbp-20261121, Antigravity); near match; names and types await human review. */

struct Sub {
    unsigned char pad[0x10];
    unsigned short cat;
};

struct Page {
    unsigned char pad[0x18];
    Sub *sub;
};

struct Ui {
    unsigned char pad[0x1c];
    Page **begin;
    Page **end;

    inline Page *top()
    {
        if (this == 0 || begin == end) {
            return 0;
        }
        return end[-1];
    }
};

struct Entry {
    int f0;
    int kind;
    int pad[11];
};

struct Mgr {
    int pad1[2];
    int mode;
    unsigned char pad2[0x160 - 0xc];
    Entry *entries;
};

extern "C" Ui *DAT_08beee0c;
extern "C" void *DAT_08beed8c;
extern "C" int FUN_088e1d90(void *);

extern "C" int FUN_088db95c(Mgr *param_1)
{
    if (param_1->entries[DAT_08beee0c->top()->sub->cat].kind == 2) {
        return 1;
    }
    if (param_1->entries[DAT_08beee0c->top()->sub->cat].kind == 1 && param_1->mode == 2 &&
        FUN_088e1d90(DAT_08beed8c) != 0) {
        return 1;
    }
    return 0;
}

// Near-miss note: Null-check/CSE diff on repeated top() call.
// near miss from lane oc-lbp-20261121 (unreviewed)
