/* LBPMATCH module=EBOOT.BIN start=0x0895edcc end=0x0895eea8 symbol=FUN_0895edcc lang=c99 rodata=0x08bb8238 */
/* Hand-written reconstruction from the disassembly (lane api-lbp-b-20261121, claude-sonnet-5-5); exact match verified; names and types await human review. */
struct VT { int pad[30]; void (*f78)(void *); void (*f7c)(void *); };
struct Node { const struct VT *vt; };
struct Tgt { char pad0[336]; float f150; float f154; float f158; int f15c; int f160; };
struct Ent { char pad0[292]; struct Tgt *tgt; char pad1[40]; float f150; };
struct Slot { int pad0[3]; struct Ent *ent; int pad1[6]; };
struct World { char pad0[336]; struct Slot *slots; char pad1[259]; unsigned char flag; };
struct Key { unsigned flags; char pad[120]; unsigned short idx; };
extern struct World *DAT_08beed14;
extern struct Key *FUN_088d7a48(struct World *w, int key);

static inline void scale(struct Slot *s)
{
  struct Tgt *t = s->ent->tgt;
  float f = s->ent->f150;
  t->f160 = 0;
  f = f * 1.25f;
  t->f154 = f;
  t->f158 = f - f;
  t->f150 = f;
}

void FUN_0895edcc(int unused, int key)
{
  struct Key *k = FUN_088d7a48(DAT_08beed14, key);
  struct Slot *s;
  struct Node *n;
  if (k == 0)
    return;
  s = DAT_08beed14->slots + k->idx;
  if (s == 0)
    return;
  if (s->ent == 0)
    return;
  n = (struct Node *)s->ent->tgt;
  if (n == 0)
    return;
  if (DAT_08beed14->flag == 0 && (k->flags & 0x2000) == 0) {
    n->vt->f78(n);
    scale(s);
    return;
  }
  n->vt->f7c(n);
  scale(s);
}
// NEAR: 30 words differ (51/55 words). Retail lays the 0x78-call path first with the float tail inline (fallthrough) and the 0x7c-call block
// out of line after the epilogue, jumping back into the tail (`j 0x895ee54`); ours puts the 0x7c block between and jumps over it. gcc404xs-O2 duplicates the tail instead (38 differ).
// Tried: single if/else with a shared tail, inline helper for the tail, early-return arms.
// near miss from lane api-lbp-b-20261121 (unreviewed)
