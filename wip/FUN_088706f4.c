/* LBPMATCH module=EBOOT.BIN start=0x088706f4 end=0x088707ac symbol=FUN_088706f4 lang=c99 */
/* Hand-written reconstruction from the disassembly (lane api-sonnet-20261121, Claude Sonnet 5.5); exact match verified; names and types await human review. */
extern void FUN_08a9d390(void *buf, unsigned v);
extern void FUN_08a9bff8(void *buf);
extern unsigned **FUN_08870680(void *map, unsigned *key);
unsigned FUN_088706f4(unsigned *hdr, int base, int ctx)
{
  unsigned tag = *hdr;
  unsigned alt;
  if (tag != 0x4154454d) {
    alt = 0x20475441;
    if (tag > 0x4154454d) {
      if (tag == 0x4d47414d) {
        unsigned off = hdr[1];
        unsigned key[2];
        unsigned buf[6];
        unsigned **slot;
        key[0] = *(unsigned *)(base + off);
        FUN_08a9d390(buf, key[0]);
        FUN_08a9bff8(buf);
        slot = FUN_08870680((void *)(ctx + 0x6c), key);
        *slot = (unsigned *)(base + off) + 3;
        return 0;
      }
      alt = 0x53544e45;
    }
    if (tag != alt)
      return 0;
  }
  return 1;
}
// NEAR: 32 words differ (47 vs 46 words). Control flow matches Ghidra's uVar1 form, but retail returns directly in v0
// (alt constant in v0, return 0 via bnel delay slot) while ours merges returns into a0 + final move v0,a0.
// Ruled out: switch (several orders), if-chain with tag > META, ret variable, return tag == alt.
// near miss from lane api-sonnet-20261121 (unreviewed)
