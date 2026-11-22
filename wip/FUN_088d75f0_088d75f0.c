/* LBPMATCH module=EBOOT.BIN start=0x088d75f0 end=0x088d76bc symbol=FUN_088d75f0 lang=c99 */
/* Hand-written reconstruction from the disassembly (lane oc-lbp-20261121, Antigravity); near match; names and types await human review. */

struct EntryA {
    int pad1[4];
    int f10;
    int pad2[5];
    int kind;
    unsigned char pad3[0x1cc - 0x2c];
};

struct EntryB {
    unsigned int pad0 : 3;
    unsigned int flag : 1;
    unsigned int rest : 28;
    int pad[6];
};

struct GlobalB {
    int f0;
    struct EntryB *arr;
};

struct Obj {
    unsigned char pad[0x1bc];
    struct EntryA *arr;
    unsigned int count;
};

extern struct GlobalB *DAT_08bedb74;
extern unsigned int DAT_08bdb348;
extern void FUN_088d283c(void);
extern void FUN_088d2918(unsigned int);

void FUN_088d75f0(struct Obj *param_1)
{
    unsigned int i;
    FUN_088d283c();
    for (i = 0; i < param_1->count; i++) {
        unsigned int flag = DAT_08bedb74->arr[i].flag;
        struct EntryA *a = &param_1->arr[i];
        int kind = a->kind;
        if (a->f10 == 0 && !flag) {
            if (kind == 5) {
                FUN_088d2918(i);
            } else if (kind == 3) {
                DAT_08bdb348 = i;
            }
        }
    }
}

// Near-miss note: Bitfield ext/mask and loop scheduling diff on 51/51 words; both C and C++ bool bitfield tried.
// near miss from lane oc-lbp-20261121 (unreviewed)
