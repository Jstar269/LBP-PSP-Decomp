/* LBPMATCH module=EBOOT.BIN start=0x0896f8d0 end=0x0896f964 symbol=FUN_0896f8d0 lang=c99 */
/* Hand-written reconstruction from the disassembly (lane api-lbp-b-20261121, claude-sonnet-5-5); exact match verified; names and types await human review. */
struct Item { char pad[12]; unsigned kind; };
struct Stack { int *base; short idx; };
struct World {
  char pad0[16];
  struct Stack *stk;
  char pad1[36];
  struct Item **tab;
  short cur;
};
static inline short count(struct Stack *s)
{
  return s->idx + 1;
}

struct Obj { char pad[140]; unsigned char flag; };
extern struct World *DAT_08bf03b8;

void FUN_0896f8d0(struct Obj *o, unsigned char v)
{
  struct World *w = DAT_08bf03b8;
  struct Stack *s = w->stk;
  if (count(s) != 0 && s->base[s->idx] != 0) {
    if (w->cur == -1) {
      o->flag = 0;
    } else {
      struct Item *it = w->tab[w->cur];
      if (it == 0) {
        o->flag = 0;
      } else {
        switch (it->kind) {
        case 2:
        case 3:
        case 5:
        case 6:
        case 7:
        case 9:
        case 10:
        case 16:
        case 17:
        case 18:
          o->flag = v;
          break;
        default:
          o->flag = 0;
          break;
        }
      }
    }
  }
}
// NEAR: 34 words differ (37/37). The retail structure is the same (count test via (short)(idx + 1), item kind tested as a bit set 0x706ec = switch cases 2,3,5,6,7,9,10,16,17,18, shared `sb zero,140` exits);
// retail computes idx << 2 into a0 before the count test (so `o` is first copied to a3), then reaches the zero-store exit through beq/beqz to 0x958; ours keeps o in a0 and orders the exits differently.
// Tried: if/else with a mask expression (srav form), switch (bit-test form), idx as a local int (folds the +1 test), count() helper.
// near miss from lane api-lbp-b-20261121 (unreviewed)
