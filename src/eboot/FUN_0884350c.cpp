/* LBPMATCH module=EBOOT.BIN start=0x0884350c end=0x08843570 symbol=FUN_0884350c lang=c++  */
/* Hand-written reconstruction from the disassembly (lane lbp-decomp-b, Claude Sonnet 5.5); exact match verified; names and types await human review. */
extern "C" int FUN_08ba0d98(const char *a, const char *b);
struct Entry { int pad[5]; char *key; /* +0x14 */ int pad2[2]; Entry *next; /* +0x20 */ };
extern "C" Entry *FUN_0884350c(Entry *list, const char *name)
{
  for (Entry *e = list->next; e != list; e = e->next)
    if (FUN_08ba0d98(e->key + 8, name) == 0) return e;
  return 0;
}
