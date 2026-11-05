/* LBPMATCH module=EBOOT.BIN start=0x089ece88 end=0x089ecf0c symbol=FUN_089ece88 lang=c++ cc=gcc404xs-O2 */
/* Hand-written reconstruction from the disassembly (lbp-decomp lane 2026-11-04, Claude Opus 5.5); exact match verified; names and types await human review. */
extern "C" float DAT_08bf3684[];
extern "C" float DAT_08bdbe3c[];

extern "C" float FUN_089ece88(float x)
{
  if (DAT_08bf3684[0] < x)
    return DAT_08bdbe3c[0];
  if (DAT_08bf3684[1] < x)
    return DAT_08bdbe3c[1];
  if (DAT_08bf3684[2] < x)
    return DAT_08bdbe3c[2];
  if (DAT_08bf3684[3] < x)
    return DAT_08bdbe3c[3];
  return DAT_08bdbe3c[4];
}
