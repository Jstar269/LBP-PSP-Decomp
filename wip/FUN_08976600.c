/* LBPMATCH module=EBOOT.BIN start=0x08976600 end=0x089766a0 symbol=FUN_08976600 lang=c99 */
/* Hand-written reconstruction from the disassembly (lane api-lbp-b-20261121, claude-sonnet-5-5); exact match verified; names and types await human review. */
struct Rc { char pad[8]; int count; int pad1[2]; int lock[1]; };
struct Str { int a; int b; int c; struct Rc *rc; };
extern void *DAT_08be84a0;
extern int FUN_08adbb84(void *pool, int size);
extern void FUN_08ad3088(int *lock);
extern void FUN_08ad2810(int *lock);

int FUN_08976600(int unused, struct Str *src)
{
  int n = FUN_08adbb84(DAT_08be84a0, 24);
  struct Str *d = (struct Str *)(n + 8);
  if (d != 0) {
    struct Rc *rc = src->rc;
    int a = src->a;
    int b = src->b;
    int c = src->c;
    int *lk = rc->lock;
    d->a = a;
    d->c = c;
    d->b = b;
    d->rc = rc;
    if (rc != 0) {
      FUN_08ad3088(lk);
      rc->count = rc->count + 1;
      FUN_08ad2810(lk);
    }
  }
  return n;
}
// NEAR: 35 words differ but only through one register role (38/40 words): everything matches except that retail keeps the lock address (rc + 20) in its own callee-saved register (s3, frame 32) while src stays in s0,
// ours reuses s0 for it (frame 16). Placement of a 24-byte node + 8 payload (null test on node + 8 as `addiu a2,v0,8; beqz a2`) matches.
// Tried: lock address computed early as a local, load orders of the copied fields.
// near miss from lane api-lbp-b-20261121 (unreviewed)
