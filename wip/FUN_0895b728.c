/* LBPMATCH module=EBOOT.BIN start=0x0895b728 end=0x0895b7f0 symbol=FUN_0895b728 lang=c99 */
/* Hand-written reconstruction from the disassembly (lane api-lbp-b-20261121, claude-sonnet-5-5); exact match verified; names and types await human review. */
extern void *DAT_08bdc188;
extern void *DAT_08befb88;
extern char DAT_08befb98[];
extern int DAT_08befba0;
extern void *DAT_08befad0;
extern char DAT_08befaf4;
extern void FUN_0895a718(void *a, void *b);
extern int FUN_08b8dfd8(void *g);
extern void FUN_08b8e050(void *g);
extern void FUN_08b9ca80(void *f);
extern void FUN_0895a86c(void);
extern void FUN_08959974(void *a, void *b, void *c);

void FUN_0895b728(unsigned char *p)
{
  if (!p[173] && DAT_08befb88 && p[83] && !p[172]) {
    FUN_0895a718(DAT_08bdc188, p + 16);
    char *g = DAT_08befb98;
    if (!*g && FUN_08b8dfd8(g)) {
      DAT_08befad0 = &DAT_08befba0;
      DAT_08befba0 = 0;
      FUN_08b8e050(g);
      FUN_08b9ca80(FUN_0895a86c);
    }
    FUN_08959974(&DAT_08befba0, DAT_08befb88, &DAT_08befaf4);
  }
}
// NEAR: 38 words differ (48/50 words). Retail is a C++ function-local static (guard DAT_08befb98 byte, object DAT_08befba0, atexit(FUN_0895a86c)),
// address kept whole in s0 (addiu s0,v0,-1128 after lb), early exits as beqz+copied lw (no beqzl). Tried: early returns, && chain, g pointer, extern char[],
// gcc404xs-O2 (56 words). A real C++ static needs the atexit stub FUN_0895a86c as the compiler's own __tcf_0 (not expressible).
// near miss from lane api-lbp-b-20261121 (unreviewed)
