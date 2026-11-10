/* LBPMATCH module=EBOOT.BIN start=0x089de16c end=0x089de1a8 symbol=FUN_089de16c lang=c99 cc=gcc404xs-O2 */
/* Hand-written reconstruction from the disassembly (lane lbp6a, Claude Sonnet 5.5); unsigned loop index and statement order found by lbpnearperm.py tag np85c; names and types await human review. */
struct Slot { char pad[0x18]; int f18; };
struct Obj { char pad[0x40]; struct Slot *slots[4]; unsigned char state[4]; };
void FUN_089de16c(struct Obj *o)
{
  unsigned int i;
  if (o->slots[0] != 0) {
    for (i = 0; i < 4; i++) {
      o->state[i] = 2;
      struct Slot *s = o->slots[i];
      s->f18 = 0;
    }
  }
}
