/* LBPMATCH module=EBOOT.BIN start=0x0889b0e8 end=0x0889b1ac symbol=FUN_0889b0e8 lang=c++ */
/* Hand-written reconstruction from the disassembly (lane lbp17c, Codex GPT-6); exact match verified; matching idiom: assign the Vec view in the loop condition to preserve the per-iteration address calculation; names and types await human review. */
struct Vec { void **start; void **end; void **limit; };
struct Buffer { void **start; void **end; };
struct State { int prefix[3]; Vec items; Buffer pending; };
struct Item {
    virtual void unused0();
    virtual void unused1();
    virtual void unused2();
};
extern "C" void FUN_0889aba0(Vec *, void **, void **);
extern "C" void *FUN_0889a97c(Buffer *, char *, char *);

extern "C" void FUN_0889b0e8(State *self)
{
    Vec *items;
    Buffer *pending = &self->pending;
    void **current;

    for (current = pending->start; ((items = &self->items), current != pending->end); current++) {
        if (items->end != items->limit) {
            if (items->end != 0)
                *items->end = *current;
            items->end++;
        } else {
            FUN_0889aba0(items, items->end, current);
        }
    }
    FUN_0889a97c(pending, (char *)pending->start, (char *)current);
    for (current = self->items.start; current != items->end; current++)
        ((Item *)*current)->unused2();
}


