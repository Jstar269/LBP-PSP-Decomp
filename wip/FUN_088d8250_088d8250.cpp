/* LBPMATCH module=EBOOT.BIN start=0x088d8250 end=0x088d82e0 symbol=FUN_088d8250 lang=c++ */
/* Hand-written reconstruction from the disassembly (lane oc-lbp-20261121, Antigravity); near match; names and types await human review. */

struct Desc {
    unsigned char pad[0x16];
    unsigned short f16;
};

struct Slot {
    unsigned short desc_idx;
    unsigned char pad[38];
};

struct StartEntry {
    unsigned short start;
    unsigned short pad;
};

struct Mgr {
    unsigned char pad1[0x148];
    Desc *descs;
    int pad14c;
    Slot *slots;
    unsigned char pad2[0x168 - 0x154];
    int counts[4];
    StartEntry starts[4];
};

extern "C" unsigned short FUN_088d8250(Mgr *param_1, int param_2, unsigned int param_3)
{
    unsigned short start = param_1->starts[param_2].start;
    unsigned short end = (unsigned short)(start + param_1->counts[param_2] - 1);
    unsigned short res = 0xffff;
    for (unsigned short i = start; i <= end; i++) {
        Slot *s = &param_1->slots[i];
        if (s != 0) {
            Desc *d = &param_1->descs[s->desc_idx];
            if (d != 0 && d->f16 == param_3) {
                res = i;
                break;
            }
        }
    }
    return res;
}

// Near-miss note: Register/branch scheduling diff on 35/36 words.
// near miss from lane oc-lbp-20261121 (unreviewed)
