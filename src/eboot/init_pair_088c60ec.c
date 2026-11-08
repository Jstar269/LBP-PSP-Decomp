/* LBPMATCH module=EBOOT.BIN start=0x088c60ec end=0x088c6180 symbol=FUN_088c60ec lang=c99 */
/* Clone of src/eboot/init_pair_088bbab8.c (lbpclone.py tag d1): addresses substituted; exact match verified; names and types await human review. */
/* Hand-written reconstruction from the disassembly (lane fb-near1, Claude); names and types await human review. */

extern char *DAT_08bedb1c;
extern void FUN_08ad3088(void *), FUN_08ad2810(void *);

void FUN_088c60ec(void)
{
  char *p, *lock;
  int i;

  for (i = 0; i != 2; i++) {
    p = DAT_08bedb1c;
    lock = p + 0x614;
    FUN_08ad3088(lock);
    FUN_08ad3088(lock);
    *(char *)(i + (int)p + 0x6b0) = 0;
    FUN_08ad2810(lock);
    FUN_08ad2810(lock);
  }
  p = DAT_08bedb1c;
  lock = p + 0x614;
  FUN_08ad3088(lock);
  p[0x6f4] = 0;
  FUN_08ad2810(lock);
}
