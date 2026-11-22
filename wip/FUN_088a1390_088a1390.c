/* LBPMATCH module=EBOOT.BIN start=0x088a1390 end=0x088a1444 symbol=FUN_088a1390 lang=c99 */
/* Hand-written reconstruction from the disassembly (lane oc-lbp-20261121, Antigravity); exact match verified; names and types await human review. */

extern void *FUN_0889d194(void);
extern int FUN_0889d094(void *);
extern float FUN_088a0560(void *);

struct Sub {
    int pad[5];
    float *arr;
};

struct Obj {
    int pad0[3];
    struct Sub sub;
    float f24;
    unsigned char pad1[272 - 40];
    void **f110;
    unsigned char pad2[312 - 276];
    int f138;
    int pad3[2];
    int f144;
};

float FUN_088a1390(struct Obj *self)
{
    if (self->f110 != 0) {
        if (self->f138 == 3) {
            int idx = self->f144;
            struct Sub *sub = &self->sub;
            const float *p;
            if (idx >= 0 && idx < FUN_0889d094(FUN_0889d194())) {
                p = &sub->arr[idx];
            } else {
                p = &self->f24;
            }
            return *p;
        }
        if (self->f138 == 4) {
            void *p = *self->f110;
            if (p != 0) {
                return FUN_088a0560(p);
            }
        }
    }
    return 0.0f;
}

// Remaining diffs (46/45 words; 28 differ): stack layout / register allocation around local struct copy and store scheduling.
// near miss from lane oc-lbp-20261121 (unreviewed)
