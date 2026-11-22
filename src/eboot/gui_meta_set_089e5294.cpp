/* LBPMATCH module=EBOOT.BIN start=0x089e5294 end=0x089e533c symbol=FUN_089e5294 lang=c++ bss=0x08bf3208 */
/* Hand-written reconstruction from the disassembly (lane api-lbp-d-20261121, claude-sonnet-5-5); exact match verified; names and types await human review. */
extern "C" int FUN_08ba0e8c(const char *name);
extern "C" void FUN_08ba0e04(char *dst, const char *name);
extern "C" void FUN_08ba08f4(char *dst, const char *s, int v);
extern "C" void FUN_08ba0be0(char *dst, const char *s);
extern const char *DAT_08bdbd38;
extern char DAT_08bc1e58[];
extern char DAT_08bc1e5c[];
struct Src { char pad[32]; int f32; };
extern "C" void FUN_089e5294(Src *s, char *dst)
{
  static int off = FUN_08ba0e8c(DAT_08bdbd38);
  FUN_08ba0e04(dst, DAT_08bdbd38);
  FUN_08ba08f4(dst + off, DAT_08bc1e58, s->f32);
  FUN_08ba0be0(dst, DAT_08bc1e5c);
}
