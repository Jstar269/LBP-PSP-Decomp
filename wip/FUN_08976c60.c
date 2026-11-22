/* LBPMATCH module=EBOOT.BIN start=0x08976c60 end=0x08976d18 symbol=FUN_08976c60 lang=c99 */
/* Hand-written reconstruction from the disassembly (lane api-lbp-b-20261121, claude-sonnet-5-5); exact match verified; names and types await human review. */
struct Lock { int x; };
extern struct Lock DAT_08bef8f4;
extern void FUN_08ad3088(struct Lock *l);
extern void FUN_08ad2810(struct Lock *l);

void FUN_08976c60(int *self, int *src, int d, int f, int g)
{
  int *rc;
  int b, c;
  self[0] = src[0];
  rc = (int *)src[3];
  c = src[2];
  b = src[1];
  self[3] = (int)rc;
  self[1] = b;
  self[2] = c;
  if (rc != 0) {
    FUN_08ad3088(&DAT_08bef8f4);
    *rc = *rc + 1;
    FUN_08ad2810(&DAT_08bef8f4);
  }
  self[4] = d;
  *((unsigned char *)self + 24) = f;
  *((unsigned char *)self + 26) = g;
  *(short *)((char *)self + 30) = 0;
  self[5] = 0;
  *((unsigned char *)self + 25) = 0;
  *(short *)((char *)self + 28) = 0;
}
// NEAR: 9 words differ (46/46). int-array views of both objects (typed struct fields hoist the loads above the first store), lock as an object. Left: the order of the loads of src[1]/src[3] and the stores of the copied words
// (retail: [3] first, `beqz s1` delay slot holds the store of word 2) and the order of the four trailing zero stores (sh 30, sw 20, sb 25, sh 28 in retail; ours emits sh 28 before sh 30).
// near miss from lane api-lbp-b-20261121 (unreviewed)
