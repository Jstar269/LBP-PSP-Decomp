/* LBPMATCH module=EBOOT.BIN start=0x0895d128 end=0x0895d1c4 symbol=FUN_0895d128 lang=c99 */
/* Hand-written reconstruction from the disassembly (lane api-lbp-b-20261121, claude-sonnet-5-5); exact match verified; names and types await human review. */
struct Arr { int pad; char *base; unsigned count; };
struct Pair { void *p; int x; };
extern int FUN_08a9b8a4(struct Pair *s, void *p);
extern int FUN_08a9d194(void *a, struct Pair *s);

unsigned FUN_0895d128(struct Arr *self, void *a)
{
  struct Pair st;
  unsigned i;
  int off = 0;
  for (i = 0; i < self->count; i++) {
    void *p = self->base + off + 4;
    int r = FUN_08a9b8a4(&st, p);
    off += 0x60;
    st.p = p;
    st.x = r;
    if (FUN_08a9d194(a, &st) != 0)
      break;
  }
  return i;
}
