/* LBPMATCH module=EBOOT.BIN start=0x0889de48 end=0x0889df18 symbol=FUN_0889de48 lang=c++ */
/* Hand-written reconstruction from the disassembly (lane oc-lbp-20261121, Antigravity); exact match verified; names and types await human review. */

struct Item {
    int a[6];
};

extern "C" void FUN_08a9c5b0(Item *, Item *);
extern "C" void FUN_08a9bff8(Item *);

struct Vec {
    Item *begin;
    Item *end;
    Item *cap;
};

extern "C" Item *FUN_0889de48(Vec *self, Item *first, Item *last)
{
    Item *dst = first;
    Item *src = last;
    int n = self->end - last;
    for (; n > 0; n--, dst++, src++) {
        FUN_08a9c5b0(dst, src);
    }
    for (Item *e = self->end; dst != e; dst++) {
        FUN_08a9bff8(dst);
    }
    self->end -= (last - first);
    return first;
}
// near miss from lane oc-lbp-20261121 (unreviewed)
