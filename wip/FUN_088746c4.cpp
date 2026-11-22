/* LBPMATCH module=EBOOT.BIN start=0x088746c4 end=0x08874754 symbol=FUN_088746c4 lang=c++ */
/* Hand-written reconstruction from the disassembly (lane api-sonnet-20261121, Claude Sonnet 5.5); exact match verified; names and types await human review. */
struct Lock { int x; };
extern "C" Lock DAT_08bef8f4;
extern "C" void FUN_08ad3088(void *lock);
extern "C" void FUN_08ad2810(void *lock);
extern "C" void FUN_0888256c();
struct Self {
  virtual void v0();
  virtual void destroy();
  int refs;   /* +4 */
  int cnt;    /* +8 */
  char f0c;   /* +0x0c */
  char pad[7];
  char f14;   /* +0x14 */
  char pad2[2];
  char f17;   /* +0x17 */
};
struct Info { char pad[0x40]; int flag; };
struct Owner { Info *info; };
extern "C" void FUN_088746c4(Owner *o, Self *self)
{
  if (*(int *)((char *)*(void **)o + 0x40) != 0)
    self->f0c = 1;
  self->f14 = 1;
  self->f17 = 0;
  FUN_08ad3088(&DAT_08bef8f4);
  int r = self->refs - 1;
  self->cnt = self->cnt - 1;
  self->refs = r;
  FUN_08ad2810(&DAT_08bef8f4);
  if (r == 0)
    self->destroy();
  FUN_0888256c();
}
// NEAR: 22 words differ, all from one missing nop: retail has `sb zero,23(s1); jal 0x8ad3088; nop` (delay slot unfilled), ours fills the slot with that sb.
// Ruled out: f17/f14 store order, bool members, raw (char*)self+0x17 store, extra padding member.
// near miss from lane api-sonnet-20261121 (unreviewed)
