/* LBPMATCH module=EBOOT.BIN start=0x08970b6c end=0x08970c00 symbol=FUN_08970b6c lang=c99 */
/* Hand-written reconstruction from the disassembly (lane api-lbp-b-20261121, claude-sonnet-5-5); exact match verified; names and types await human review. */
struct Obj;
struct VT { void (*dtor)(struct Obj *); void (*del)(struct Obj *); };
struct Obj { const struct VT *vt; short size; short rc; };
extern void *DAT_08be84a0;
extern void FUN_08adbc24(void *pool, struct Obj *o, int size);

void FUN_08970b6c(struct Obj *o)
{
  short n = o->rc - 1;
  o->rc = n;
  if (n == 0) {
    int size = o->size;
    if ((unsigned)size < 25) {
      o->vt->dtor(o);
      FUN_08adbc24(DAT_08be84a0, o, size);
    } else {
      o->vt->del(o);
    }
  }
}
