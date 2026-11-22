/* LBPMATCH module=EBOOT.BIN start=0x088d3344 end=0x088d3430 symbol=FUN_088d3344 lang=c99 */
/* Hand-written reconstruction from the disassembly (lane oc-lbp-20261121, Antigravity); near match; names and types await human review. */

struct Item {
    unsigned char pad1[0x28];
    int kind;
    int f2c;
    int f30;
    int f34;
    int f38;
    char f3c[1];
};

extern void *DAT_08be85fc;
extern char DAT_08bb4318[];
extern short DAT_08beecf0;
extern void *FUN_08970dbc(void *, int);
extern void FUN_088399bc(void *, int, const char *, const char *);

static inline void *lookup(int id)
{
    if (id != 900) {
        return FUN_08970dbc(DAT_08be85fc, id);
    }
    return 0;
}

void *FUN_088d3344(struct Item *param_1, int param_2)
{
    switch (param_2) {
    case 0: {
        int k = param_1->kind;
        if (k == 3 || k == 5 || k == 4) {
            return lookup(param_1->f2c);
        }
        return (void *)param_1->f2c;
    }
    case 1: {
        int k = param_1->kind;
        if (k == 3 || k == 5 || k == 4) {
            return lookup(param_1->f30);
        }
        return (void *)param_1->f30;
    }
    case 2: {
        int k = param_1->kind;
        if (k == 3 || k == 5 || k == 4) {
            return lookup(param_1->f34);
        }
        FUN_088399bc(&DAT_08beecf0, 0x11, DAT_08bb4318, param_1->f3c);
        return &DAT_08beecf0;
    }
    default:
        return 0;
    }
}

// Near-miss note: Register/branch layout diff on 59/59 words.
// near miss from lane oc-lbp-20261121 (unreviewed)
