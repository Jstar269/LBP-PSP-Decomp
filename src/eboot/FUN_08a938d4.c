/* LBPMATCH module=EBOOT.BIN start=0x08a938d4 end=0x08a938e0 symbol=FUN_08a938d4 lang=c99 cc=gcc410-Os */
/* Clone of src/eboot/FUN_08804764.c (lbpclone.py tag b2): addresses substituted; exact match verified; names and types await human review. */

/* Thin wrapper: forwards fixed arguments (0, 0xffff) to FUN_08a93684 (a sibling wrapper passes 1 first). */
extern void FUN_08a93684(int flag, unsigned int mask);

void FUN_08a938d4(void)
{
  FUN_08a93684(0, 0xffff);
}
