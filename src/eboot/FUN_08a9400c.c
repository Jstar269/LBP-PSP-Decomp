/* LBPMATCH module=EBOOT.BIN start=0x08a9400c end=0x08a94018 symbol=FUN_08a9400c lang=c99 cc=gcc410-Os */
/* Clone of src/eboot/FUN_08804770.c (lbpclone.py tag b2): addresses substituted; exact match verified; names and types await human review. */

/* Thin wrapper: forwards fixed arguments (1, 0xffff) to FUN_08a93db0 (a sibling wrapper passes 0 first). */
extern void FUN_08a93db0(int flag, unsigned int mask);

void FUN_08a9400c(void)
{
  FUN_08a93db0(1, 0xffff);
}
