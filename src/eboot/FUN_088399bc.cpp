/* LBPMATCH module=EBOOT.BIN start=0x088399bc end=0x08839a1c symbol=FUN_088399bc lang=c++  */
/* Hand-written reconstruction from the disassembly (lane lbp-decomp-b, Claude Sonnet 5.5); exact match verified; names and types await human review. */
extern "C" void FUN_08af5d0c(unsigned short *buf, int n, const char *fmt, __builtin_va_list ap);
extern "C" void FUN_088399bc(unsigned short *buf, int n, const char *fmt, ...)
{
  __builtin_va_list ap;
  __builtin_va_start(ap, fmt);
  int last = n - 1;
  FUN_08af5d0c(buf, last, fmt, ap);
  buf[last] = 0;
}
