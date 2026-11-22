/* LBPMATCH module=EBOOT.BIN start=0x088d2480 end=0x088d2548 symbol=FUN_088d2480 lang=c99 */
/* Hand-written reconstruction from the disassembly (lane oc-lbp-20261121, Antigravity); near match; names and types await human review. */

struct Holder {
    int f0;
    int f4;
    unsigned int count1;
    unsigned int count2;
    int f10;
    unsigned char dirty;
    unsigned char pad1[3];
    int arr1[250];
    int arr2[82];
};

struct Item {
    int pad[9];
    int val;
    int kind;
};

void FUN_088d2480(struct Holder *param_1, struct Item *param_2)
{
    if (param_2 == 0) {
        return;
    }
    if (param_2->kind == 1) {
        unsigned int n = param_1->count1;
        int val = param_2->val;
        if (n < 250) {
            unsigned int i = 0;
            while (i < n) {
                if (param_1->arr1[i] == val) {
                    return;
                }
                i++;
            }
            param_1->arr1[n] = val;
            param_1->count1 = n + 1;
            param_1->dirty = 1;
        }
    } else if (param_2->kind == 0) {
        unsigned int n = param_1->count2;
        int val = param_2->val;
        if (n < 82) {
            unsigned int i = 0;
            while (i < n) {
                if (param_1->arr2[i] == val) {
                    return;
                }
                i++;
            }
            param_1->arr2[n] = val;
            param_1->count2 = n + 1;
            param_1->dirty = 1;
        }
    }
}

// Near-miss note: Loop/register allocation diff on 50/50 words.
// near miss from lane oc-lbp-20261121 (unreviewed)
