/* LBPMATCH module=EBOOT.BIN start=0x088e1b10 end=0x088e1bf8 symbol=FUN_088e1b10 lang=c++ */
/* Hand-written reconstruction from the disassembly (lane oc-lbp-20261121, Antigravity); near match; names and types await human review. */

struct Entry {
    unsigned char pad[0x3c];
    unsigned char f3c;
    unsigned char f3d;
};

struct Mgr {
    unsigned char pad[0x1c4];
    Entry *f1c4;
};

struct Query {
    int kind;
    unsigned char flag;
    unsigned char pad[3];
    unsigned int f8;
    unsigned int f0c;
    unsigned int f10;
};

extern "C" Mgr *DAT_08beed14;
extern "C" int FUN_08ba0d98(Entry *, Entry *);

extern "C" bool FUN_088e1b10(Query *param_1, Entry *param_2)
{
    if (param_1->flag == 0) {
        return (param_1->f8 * param_1->f0c) < param_1->f10;
    }
    if (param_2 != 0) {
        Entry *e = DAT_08beed14->f1c4;
        if (FUN_08ba0d98(param_2, e) == 0) {
            param_2 = e;
        }
    }
    if (param_1->kind == 0x10) {
        if (param_2 == 0) {
            return false;
        }
        return (param_1->f8 * param_1->f0c) < param_2->f3d;
    }
    if (param_1->kind == 0x11) {
        if (param_2 == 0) {
            return false;
        }
        return (param_1->f8 * param_1->f0c) < param_2->f3c;
    }
    return param_1->f0c <= param_1->f10;
}

// Near-miss note: Branch/regalloc diff on 54/58 words.
// near miss from lane oc-lbp-20261121 (unreviewed)
