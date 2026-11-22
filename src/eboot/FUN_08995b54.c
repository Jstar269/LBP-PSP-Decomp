/* LBPMATCH module=EBOOT.BIN start=0x08995b54 end=0x08995c04 symbol=FUN_08995b54 lang=c99 */
/* Hand-written reconstruction from the disassembly (lane api-lbp-b-20261121, claude-sonnet-5-5); exact match verified; names and types await human review. */
struct G { char pad[4]; unsigned char on; };
extern struct G *DAT_08be8480;
extern void *DAT_08befcac;
extern void *DAT_08bedb54;
extern void FUN_0895cce4(void *a, int b);
extern int FUN_0884224c(void *a, int b);

void FUN_08995b54(void)
{
  if (DAT_08be8480 != 0 && DAT_08be8480->on != 0) {
    FUN_0895cce4(DAT_08befcac, 22);
    if (FUN_0884224c(DAT_08bedb54, 23) != 0)
      FUN_0895cce4(DAT_08befcac, 24);
    if (FUN_0884224c(DAT_08bedb54, 25) != 0)
      FUN_0895cce4(DAT_08befcac, 26);
    if (FUN_0884224c(DAT_08bedb54, 34) != 0)
      FUN_0895cce4(DAT_08befcac, 38);
  }
}
