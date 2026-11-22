/* LBPMATCH module=EBOOT.BIN start=0x0895bfc4 end=0x0895c08c symbol=FUN_0895bfc4 lang=c99 */
/* Hand-written reconstruction from the disassembly (lane api-lbp-b-20261121, claude-sonnet-5-5); exact match verified; names and types await human review. */
struct G { int pad[2]; int a; int b; };
extern struct G *DAT_08befb88;
extern void FUN_0895b95c(void *p, int a, int b);
extern void FUN_08afd1e0(void *g, int a, int b);
extern void FUN_08afcf4c(void *g);

void FUN_0895bfc4(unsigned char *p)
{
  unsigned char c;
  if (p[173] != 0)
    return;
  if (p[80] == 0)
    return;
  if (DAT_08befb88 == 0)
    return;
  if (p[83] == 0)
    return;
  if (p[172] != 0)
    return;
  if (DAT_08befb88->b == DAT_08befb88->a) {
    c = p[169];
  } else {
    if (p[82] == 0) {
      c = p[81];
      goto L;
    }
    c = p[169];
  }
  if (c == 0) {
    FUN_0895b95c(p, 0, 0);
    return;
  }
  FUN_08afd1e0(DAT_08befb88, 0, 0);
  c = p[81];
L:
  if (c != 0)
    return;
  FUN_08afcf4c(DAT_08befb88);
}
// NEAR: 37 words differ (48/50 words). Layout matches retail; retail keeps the global in a1 (p stays in a0 for the early loads via regcprop, s0 copy only for the late ones), ours puts it in a0 and loads through s0.
// Tried: local g, DAT re-read, inline ready() helper (50 words, layout worse, 36 differ).
// near miss from lane api-lbp-b-20261121 (unreviewed)
