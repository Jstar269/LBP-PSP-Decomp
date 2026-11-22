/* LBPMATCH module=EBOOT.BIN start=0x0894967c end=0x08949714 symbol=FUN_0894967c lang=c99 */
/* Hand-written reconstruction from the disassembly (lane api-lbp-a-20261121, Claude Sonnet 5.5); exact match verified; names and types await human review. */
extern void FUN_08a9e0c4(void *);
extern void *FUN_08a9e824(unsigned);
extern void FUN_08b9eb88(void *, int, unsigned);
struct S { char pad[44]; unsigned count; int f48; void *data; };
void FUN_0894967c(struct S *s, unsigned n)
{
    if (s->data) {
        FUN_08a9e0c4(s->data);
        s->count = 0;
        s->data = 0;
        s->f48 = 0;
    }
    s->count = n;
    if (n) {
        s->data = FUN_08a9e824(n * 72);
        FUN_08b9eb88(s->data, 0, s->count * 72);
    }
}
