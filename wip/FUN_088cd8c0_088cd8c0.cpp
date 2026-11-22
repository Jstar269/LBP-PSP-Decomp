/* LBPMATCH module=EBOOT.BIN start=0x088cd8c0 end=0x088cd950 symbol=FUN_088cd8c0 lang=c++ */
/* Hand-written reconstruction from the disassembly (lane oc-lbp-20261121, Antigravity); near match; names and types await human review. */

struct Obj {
    unsigned char pad1[0x3c];
    unsigned char count;
    unsigned char pad2[0x50 - 0x3d];
    int arr[250];
};

struct Req {
    int pad[8];
    int kind;
    int val;
};

extern "C" bool FUN_088cd8c0(Obj *self, Req *param_2)
{
    unsigned char count = self->count;
    int val = param_2->val;
    if (param_2->kind != 1) {
        for (short i = 249; i >= count; i--) {
            int v = self->arr[i];
            if (v == val) {
                return true;
            }
            if (v == 0) {
                return false;
            }
        }
        return false;
    }
    short i;
    for (i = 0; i < count; i++) {
        if (self->arr[i] == val) {
            return true;
        }
    }
    return i < count;
}

// Near match: 34/36 words; 35 differ.
// Body instructions match; missing 2 words vs retail:
// 1. After lbu a2,60(a0) retail emits redundant `andi a3,a2,0xff; move a2,a3`.
// 2. Retail emits `jr ra; andi v0,v0,0xff` on the second loop return.
// Ruled out: gcc404xs-O2 (45 words), unsigned char return, inline helper with count & 0xff.
// near miss from lane oc-lbp-20261121 (unreviewed)
