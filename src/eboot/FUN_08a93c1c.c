/* LBPMATCH module=EBOOT.BIN start=0x08a93c1c end=0x08a93c28 symbol=FUN_08a93c1c lang=c99 cc=gcc410-Os */
/* Clone of src/eboot/FUN_08804770.c (lbpclone.py tag b2): addresses substituted; exact match verified; names and types await human review. */

/* Thin wrapper: forwards fixed arguments (1, 0xffff) to FUN_08a939c0 (a sibling wrapper passes 0 first). */
extern void FUN_08a939c0(int flag, unsigned int mask);

void FUN_08a93c1c(void)
{
  FUN_08a939c0(1, 0xffff);
}
