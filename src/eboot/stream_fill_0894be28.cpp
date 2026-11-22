/* LBPMATCH module=EBOOT.BIN start=0x0894be28 end=0x0894bec8 symbol=FUN_0894be28 lang=c++ */
/* Hand-written reconstruction from the disassembly (lane api-lbp-a-20261121, Claude Sonnet 5.5); exact match verified; names and types await human review. */
struct Stream {
    int pad0;
    unsigned char *buf;
    unsigned char *cur;
    unsigned high;
    unsigned pos;
    void put(const void *pv)
    {
        if (buf) {
            *cur = *(const unsigned char *)pv;
            unsigned p = pos + 1;
            cur = buf + p;
            pos = p;
        } else {
            unsigned p = pos + 1;
            pos = p;
            if (high < p)
                high = p;
        }
    }
};
struct A {
    int pad0, pad1, pad2;
    unsigned f12;
    int mode;
    unsigned count;
};
extern "C" void FUN_0894be28(A *a, Stream *s)
{
    unsigned char b = 0;
    a->f12 = s->pos;
    switch (a->mode) {
    case 0:
        b = 0xff;
        break;
    case 1:
        b = 0;
        break;
    }
    for (unsigned n = 0; n < a->count; n++)
        s->put(&b);
}
