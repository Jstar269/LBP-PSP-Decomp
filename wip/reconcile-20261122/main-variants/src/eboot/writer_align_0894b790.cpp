/* LBPMATCH module=EBOOT.BIN start=0x0894b790 end=0x0894b888 symbol=_ZN6Writer12FUN_0894b790EP6StreamPv lang=c++ */
/* Hand-written reconstruction from the disassembly (lane lbp21e, Claude Opus 5.5); exact match verified; names and types await human review. */
struct Stream {
    int _0;
    char *base;
    char *cur;
    unsigned int size;
    unsigned int pos;
    inline void Write(const void *p, int n)
    {
        for (int i = 0; i < n; i++) {
            if (base) {
                *cur = ((const char *)p)[i];
                pos = pos + 1;
                cur = base + pos;
            } else {
                pos = pos + 1;
                if (size < pos)
                    size = pos;
            }
        }
    }
};

struct Item {
    virtual void v0();
    virtual void v1();
    virtual void Write(Stream *s, void *ctx);
};

struct Vec {
    Item **b;
    Item **e;
    Item **begin() { return b; }
    Item **end() { return e; }
};

struct Writer {
    int _0[3];
    Vec items;
    int _14;
    unsigned int align;
    unsigned int start;
    unsigned int length;
    void FUN_0894b790(Stream *s, void *ctx);
};

void Writer::FUN_0894b790(Stream *s, void *ctx)
{
    unsigned int a = align;
    char zero = 0;
    while (s->pos % a != 0)
        s->Write(&zero, 1);
    start = s->pos;
    for (Item **it = items.begin(); it != items.end(); it++)
        (*it)->Write(s, ctx);
    length = s->pos - start;
}
