/* LBPMATCH module=EBOOT.BIN start=0x08a232f8 end=0x08a23314 symbol=FUN_08a232f8 lang=c++ */
/* Hand-written reconstruction from the disassembly (lbp-decomp lane 2026-11-04, Claude Opus 5.5); exact match verified; names and types await human review. */
extern "C" unsigned int DAT_08bdc1a0;
extern "C" bool DAT_08bf6156;

extern "C" void FUN_08a232f8(unsigned int colour)
{
  DAT_08bdc1a0 = colour & 0xffffff;
  DAT_08bf6156 = true;
}
