/* LBPMATCH module=EBOOT.BIN start=0x088cf88c end=0x088cf978 symbol=FUN_088cf88c lang=c++ */
/* Hand-written reconstruction from the disassembly (lane oc-lbp-20261121, Antigravity); near match; names and types await human review. */

struct Item {
    unsigned int flags;
};

struct Meta {
    unsigned char pad1[0x120];
    unsigned int **bits;
    unsigned char pad2[0x138 - 0x124];
    int bit_idx;

    inline void set_bit(int idx, int val)
    {
        unsigned int **bp = bits;
        int w = idx / 32;
        unsigned int m = 1U << (idx % 32);
        if (val != 0) {
            (*bp)[w] |= m;
        } else {
            (*bp)[w] &= ~m;
        }
    }
};

struct Slot {
    int f0;
    int f4;
    Item *item;
    Meta *meta;
    int pad[6];
};

struct Mgr {
    unsigned char pad1[0x150];
    Slot *slots;
    unsigned char pad2[0x254 - 0x154];
    unsigned short idx;

    inline Slot *get_slot(int i)
    {
        return (i == 0xffff) ? 0 : &slots[i & 0xffff];
    }
    inline Slot *cur_slot()
    {
        return get_slot(idx);
    }
};

extern "C" Mgr *DAT_08beed14;
extern "C" void FUN_088d411c(Item *, int);

extern "C" void FUN_088cf88c(void)
{
    Slot *s = DAT_08beed14->cur_slot();
    if (s != 0 && s->item != 0) {
        int bit = (s->item->flags >> 4) & 1;
        FUN_088d411c(s->item, bit ^ 1);
        s->meta->set_bit(s->meta->bit_idx, bit == 0);
    }
}

// Near match: 59/59 words; 17 differ; first at +0x6c.
// Everything up to +0x6c (cur_slot, ext, FUN_088d411c, div/mod 32) matches 100%.
// Remaining 17 words are register allocation and duplicating `addu v1,v0,a0` inside
// both arms of set_bit (retail keeps mask in a2 via `move a2,v1` and puts `nor a1,zero,v1`
// in the bnez delay slot).
// Ruled out: direct (*bits)[w] without bp local (60/59, 38 differ), int mask.
// near miss from lane oc-lbp-20261121 (unreviewed)
