/* LBPMATCH module=EBOOT.BIN start=0x089ecf50 end=0x089ed044 symbol=FUN_089ecf50 lang=c++ cc=gcc404xs-O2 */
/* Hand-written reconstruction from the disassembly (lbp-decomp lane 2026-11-04, Claude Opus 5.5); exact match verified; names and types await human review. */
extern "C" float DAT_08bdbe3c[];
extern "C" int FUN_089ecf50(float x)
{
  if (x <= DAT_08bdbe3c[0] && x > DAT_08bdbe3c[1])
    return 0;
  if (x <= DAT_08bdbe3c[1] && x > DAT_08bdbe3c[2])
    return 1;
  if (x <= DAT_08bdbe3c[2] && x > DAT_08bdbe3c[3])
    return 2;
  if (x <= DAT_08bdbe3c[3] && x > DAT_08bdbe3c[4])
    return 3;
  if (x <= DAT_08bdbe3c[4] && x >= DAT_08bdbe3c[5])
    return 4;
  return -1;
}
