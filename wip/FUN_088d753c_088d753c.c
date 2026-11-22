/* LBPMATCH module=EBOOT.BIN start=0x088d753c end=0x088d75f0 symbol=FUN_088d753c lang=c99 */
/* Hand-written reconstruction from the disassembly (lane oc-lbp-20261121, Antigravity); near match; names and types await human review. */

struct Entry {
    int f0;
    int kind;
    unsigned int val;
    int pad[10];
};

struct Obj {
    unsigned char pad[0x160];
    struct Entry *arr;
    int count;
};

extern void *FUN_08a50fbc(void);
extern void FUN_08a52b38(void *, unsigned int);

void FUN_088d753c(struct Obj *param_1, int param_2)
{
    int target = 1;
    switch (param_2) {
    case 2:
        target = 2;
        break;
    case 3:
        target = 4;
        break;
    case 1:
        target = 3;
        break;
    }

    int count = param_1->count;
    int i;
    for (i = 0; i < count; i++) {
        if (param_1->arr[i].kind == target) {
            FUN_08a52b38(FUN_08a50fbc(), param_1->arr[i].val);
            return;
        }
    }
}

// Near-miss note: Register/loop scheduling diff on 45/45 words.
// near miss from lane oc-lbp-20261121 (unreviewed)
