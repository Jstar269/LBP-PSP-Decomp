/* LBPMATCH module=EBOOT.BIN start=0x08878ab8 end=0x08878b18 symbol=FUN_08878ab8 lang=c++  */
/* Hand-written reconstruction from the disassembly (lane lbp-decomp-b, Claude Sonnet 5.5); exact match verified; names and types await human review. */
extern "C" int FUN_08ba08f4(char *out, const char *fmt, ...);
extern char DAT_08bb07bc[];
struct Id { int pad[3]; unsigned char bytes[20]; };
extern "C" void FUN_08878ab8(Id *id, char *out)
{
  const unsigned char *src = id->bytes;
  int i = 0;
  do {
    out += FUN_08ba08f4(out, DAT_08bb07bc, *src++);
  } while (++i != 20);
}
