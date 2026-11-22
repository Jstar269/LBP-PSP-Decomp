/* LBPMATCH module=EBOOT.BIN start=0x0896f964 end=0x0896fa34 symbol=FUN_0896f964 lang=c99 */
/* Hand-written reconstruction from the disassembly (lane api-lbp-b-20261121, claude-sonnet-5-5); exact match verified; names and types await human review. */
struct Kind { char pad[12]; int kind; };
struct Item { char pad[16]; struct Kind *k; };
struct Stack { struct Item **base; short idx; };
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

int FUN_0896f964(struct World *w)
{
  struct Stack *s = w->stk;
  struct Item *cur;
  int kind;
  int r;
  if (count(s) == 0 || s->base[s->idx] == 0)
    return 0;
  cur = 0;
  if (w->cur != -1)
    cur = w->tab[w->cur];
  if (cur->k != 0)
    return 0;
  kind = -1;
  if (count(s) != 0) {
    struct Item *it = s->base[s->idx];
    if (it == 0)
      kind = -1;
    else
      kind = it->k->kind;
  }
  r = 1;
  if (kind != 18 && kind != 20 && kind != 16) {
    r = 0;
    if (kind == 17)
      r = 1;
  }
  return r;
}
// NEAR: 51 words differ (41/52). Retail re-evaluates the stack top (idx, base, item) in three inlined accessor copies with a chain of kind compares (18, 20, 16 then 17 -> beq/bne to a shared `li a0,1` exit); ours folds the repeated loads.
// Tried: one typed attempt only (count() helper, explicit chain).
// near miss from lane api-lbp-b-20261121 (unreviewed)
