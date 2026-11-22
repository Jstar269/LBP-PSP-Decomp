/* LBPMATCH module=EBOOT.BIN start=0x089762a0 end=0x0897633c symbol=FUN_089762a0 lang=c99 */
/* Hand-written reconstruction from the disassembly (lane api-lbp-b-20261121, claude-sonnet-5-5); exact match verified; names and types await human review. */
struct Lock { int x; };
extern struct Lock DAT_08bef8f4;
extern void FUN_08ad3088(struct Lock *l);
extern void FUN_08ad2810(struct Lock *l);

void FUN_089762a0(int *dst, int *src)
{
  const unsigned short *s = (const unsigned short *)src[0];
  int len = 0;
  int *rc;
  if (s == 0) {
    dst[0] = (int)s;
  } else {
    const unsigned short *p = s;
    while (*p != 0)
      p++;
    len = p - s;
    dst[0] = (int)s;
  }
  rc = (int *)src[1];
  dst[1] = len;
  dst[2] = src[0];
  dst[3] = (int)rc;
  if (rc != 0) {
    FUN_08ad3088(&DAT_08bef8f4);
    *rc = *rc + 1;
    FUN_08ad2810(&DAT_08bef8f4);
  }
}
// NEAR: 12 words differ (39/39). int-array views of both objects reproduce retail's reload of src[0] after the dst[0] store (a typed-struct version drops the reload); the lock is an object (`struct Lock`, passed by address) as in lbp9a's note.
// Retail keeps the length in a3 (`move a3,zero` before the null test) and stores the loaded value (`sw a2,0(a0)`) in the null arm too; ours folds that store to `sw zero` and keeps the length in v0.
// near miss from lane api-lbp-b-20261121 (unreviewed)
