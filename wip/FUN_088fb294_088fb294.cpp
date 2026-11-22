/* LBPMATCH module=EBOOT.BIN start=0x088fb294 end=0x088fb344 symbol=FUN_088fb294 lang=c++ rodata=0x08baf1c0 */
/* Hand-written reconstruction from the disassembly (lane oc-lbp-20261121, Antigravity); near match; names and types await human review. */

struct Item {
    int pad[3];
    void *f0c;
};

struct SubGlobal {
    int f00;
    void *arr[1];
};

struct Global {
    unsigned char pad[0x750];
    SubGlobal sub;
};

struct Pair {
    int row;
    int col;
};

struct Obj {
    unsigned char pad0[0x1c];
    Item *ptrs[2];
    unsigned char pad1[0x8c - 0x24];
    int grid[3][3];
    unsigned char pad2[0x114 - 0xb0];
    int f114[4];
    int f124;
};

extern "C" Global *DAT_08bf7304;
extern "C" void FUN_08a50fbc(Obj *);
extern "C" void FUN_088f95a4(int *, int, float);

extern "C" void FUN_088fb294(Obj *param_1, Pair *param_2)
{
    FUN_08a50fbc(param_1);
    int idx = param_1->grid[param_2->row][param_2->col];
    SubGlobal *sg = &DAT_08bf7304->sub;
    int next = 0;
    int arg = 0;
    switch (param_1->f124) {
    case 0:
        next = 1;
        arg = 0;
        break;
    case 1:
        next = 0;
        arg = 1;
        break;
    }
    param_1->ptrs[next]->f0c = sg->arr[idx];
    FUN_088f95a4(param_1->f114, arg, 0.3f);
    param_1->f124 = next;
}

// Near-miss note: 44/44 words: switch/if-else scheduling and f12/a0 hoisting differ.
// near miss from lane oc-lbp-20261121 (unreviewed)
