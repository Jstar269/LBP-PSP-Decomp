/* LBPMATCH module=EBOOT.BIN start=0x089795b4 end=0x08979620 symbol=FUN_089795b4 lang=c++ */
/* Clone of src/eboot/node_alloc_088b6b3c.cpp (lbpclone.py tag b2): addresses substituted; exact match verified; names and types await human review. */
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
extern "C" void *const DAT_08bbd910[];
extern "C" Node *FUN_08adbb84(void *pool, unsigned int size);

extern "C" Handle *FUN_089795b4(Handle *out, void *value)
{
  Node *node = FUN_08adbb84(DAT_08be84a0, 16);
  if (node) {
    node->refs = 1;
    node->value = value;
    node->vtable = DAT_08bbd910;
    node->size = 16;
  }
  out->node = node;
  return out;
}
