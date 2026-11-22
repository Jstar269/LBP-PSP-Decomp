/* LBPMATCH module=EBOOT.BIN start=0x089ab74c end=0x089ab7f0 symbol=FUN_089ab74c lang=c99 */
/* Hand-written reconstruction from the disassembly (lane api-lbp-c-20261121, claude-sonnet-5-5); exact match verified; names and types await human review. */

struct Rc { int a; int b; int count; int c; int d; char lock[4]; };
struct Elem { struct Rc *ref; int a; int b; };

extern void FUN_08ad3088(void *lock);
extern void FUN_08ad2810(void *lock);

struct Elem *FUN_089ab74c(struct Elem *first, struct Elem *last, struct Elem *result)
{
  struct Rc *r;

  for (; first != last; first++, result++) {
    if (result != 0) {
      r = first->ref;
      result->ref = r;
      if (r != 0) {
        FUN_08ad3088(r->lock);
        r->count = r->count + 1;
        FUN_08ad2810(r->lock);
      }
      result->a = first->a;
      result->b = first->b;
    }
  }
  return result;
}
