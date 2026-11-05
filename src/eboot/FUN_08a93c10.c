/* LBPMATCH module=EBOOT.BIN start=0x08a93c10 end=0x08a93c1c symbol=FUN_08a93c10 lang=c99 cc=gcc410-Os */
/* Clone of src/eboot/FUN_08804764.c (lbpclone.py tag b2): addresses substituted; exact match verified; names and types await human review. */

/* Thin wrapper: forwards fixed arguments (0, 0xffff) to FUN_08a939c0 (a sibling wrapper passes 1 first). */
extern void FUN_08a939c0(int flag, unsigned int mask);

void FUN_08a93c10(void)
{
  FUN_08a939c0(0, 0xffff);
}
