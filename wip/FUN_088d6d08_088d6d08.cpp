/* LBPMATCH module=EBOOT.BIN start=0x088d6d08 end=0x088d6da8 symbol=FUN_088d6d08 lang=c++ */
/* Hand-written reconstruction from the disassembly (lane oc-lbp-20261121, Antigravity); near match; names and types await human review. */

struct Payload {
    unsigned char pad1[0xaa];
    unsigned short title[65];
    unsigned short desc[208];
};

struct Item {
    unsigned char pad1[0x28];
    int kind;
    unsigned short *title;
    unsigned short *desc;
    int f34;
    unsigned char pad2[0xe0 - 0x38];
    Payload payload;

    inline Payload *get_payload()
    {
        if (kind == 3 || kind == 5 || kind == 4) {
            return &payload;
        }
        return &payload;
    }
};

extern "C" void FUN_088d469c(Item *);
extern "C" void FUN_08b9eb88(void *, int, int);

extern "C" void FUN_088d6d08(Item *param_1)
{
    FUN_088d469c(param_1);
    param_1->kind = 0;
    FUN_08b9eb88(&param_1->payload, 0, 0x2cc);
    param_1->title = param_1->get_payload()->title;
    param_1->desc = param_1->get_payload()->desc;
    param_1->f34 = 0;
}

// Near-miss note: GCC folds identical branch arms; 19/40 words.
// near miss from lane oc-lbp-20261121 (unreviewed)
