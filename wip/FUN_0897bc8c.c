/* LBPMATCH module=EBOOT.BIN start=0x0897bc8c end=0x0897bd8c symbol=FUN_0897bc8c lang=c99 rodata=0x08bbd980 */
/* Hand-written reconstruction from the disassembly (lane api-lbp-b-20261121, claude-sonnet-5-5); exact match verified; names and types await human review. */
struct Slot { int pad0[2]; void *p; int pad1[3]; };
struct Self {
  char pad0[8];
  struct Slot slots[1];
};
struct Self2 {
  char pad[104];
  unsigned char f104;
  char pad1[3];
  int count;
  float timer;
  int idx;
  unsigned char f120;
};
extern int DAT_08bef40c;
extern void FUN_0896b048(void *p, int a);
extern void *FUN_08914a7c(void);
extern int FUN_08914e94(void *o, int v);
extern void FUN_08913dd8(void *o, int a, int b, int c, int d, int e, int f, int g);
extern void FUN_0897b884(void *self);

void FUN_0897bc8c(float dt, struct Self2 *o)
{
  int idx;
  if (0.0f <= o->timer) {
    o->timer = o->timer - dt;
    return;
  }
  idx = o->idx;
  if (idx >= 0) {
    if (idx >= o->count) {
      o->idx = -2;
      o->f120 = 1;
      o->f104 = 0;
      FUN_0897b884(o);
      idx = o->idx;
    } else {
      void *p = ((struct Slot *)o)[idx].p;
      void *h1;
      void *h2;
      int v;
      if (p != 0)
        FUN_0896b048(p, 1);
      h1 = FUN_08914a7c();
      h2 = FUN_08914a7c();
      v = FUN_08914e94(h2, DAT_08bef40c);
      FUN_08913dd8(h1, 0, v, 9, 0, 0, 1024, 0);
      idx = o->idx;
    }
    o->idx = idx + 1;
    o->timer = 1.2f;
  }
}
// NEAR: 4 words differ (64/64): the address `o + idx * 24` is computed `sll 3; sll 5; subu; addu v1,v0,a0` in ours, `sll 5; sll 3; subu; addu a2,a0,v0` in retail (base operand first, register a2). The arm layout (idx >= count first) matches
// once the comparison is written `idx >= o->count` for the first arm; rodata=0x08bbd980 holds the 1.2f. Tried: char* arithmetic, struct-array indexing, a Slot pointer local before the compare (5 words).
// near miss from lane api-lbp-b-20261121 (unreviewed)
