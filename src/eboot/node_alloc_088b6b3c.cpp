/* LBPMATCH module=EBOOT.BIN start=0x088b6b3c end=0x088b6ba8 symbol=FUN_088b6b3c lang=c++ */
/* Hand-written reconstruction from the disassembly (decomp-opus lane 2026-11-04, Claude Opus 5.5), store order refined by lbpperm.py tag o1 (value before vtable); exact match verified; names and types await human review. */
struct Node {
  void *const *vtable;
  short size;  /* +0x4 */
  short refs;  /* +0x6 */
  void *value; /* +0x8 */
};
struct Handle {
  Node *node;
};
extern "C" void *DAT_08be84a0;
extern "C" void *const DAT_08bb3658[];
extern "C" Node *FUN_08adbb84(void *pool, unsigned int size);

extern "C" Handle *FUN_088b6b3c(Handle *out, void *value)
{
  Node *node = FUN_08adbb84(DAT_08be84a0, 16);
  if (node) {
    node->refs = 1;
    node->value = value;
    node->vtable = DAT_08bb3658;
    node->size = 16;
  }
  out->node = node;
  return out;
}
