/* LBPMATCH module=EBOOT.BIN start=0x0895f9c4 end=0x0895faa0 symbol=FUN_0895f9c4 lang=c99 */
/* Hand-written reconstruction from the disassembly (lane api-lbp-b-20261121, claude-sonnet-5-5); exact match verified; names and types await human review. */
struct Obj { char pad0[4]; unsigned char f4; char pad1; unsigned char f6; char pad2[3]; unsigned char f10; char pad3[101]; int mode; };
extern void FUN_0895eb74(struct Obj *o, unsigned a, unsigned b, int c);
extern void FUN_0895f4f0(struct Obj *o, unsigned a, unsigned b, int c);

void FUN_0895f9c4(struct Obj *o, unsigned short a, unsigned short b, int mode, int c, unsigned char d)
{
  if (o->f6 == 0)
    return;
  o->f10 = 1;
  o->f4 = d;
  o->f6 = 0;
  o->mode = mode;
  if (mode != 2) {
    if (mode != 4) {
      if (mode != 1)
        return;
      FUN_0895eb74(o, a, b, c);
      return;
    }
    FUN_0895eb74(o, a, b, c);
  }
  FUN_0895f4f0(o, a, b, c);
}
// NEAR: 9 words differ (55/55). Retail copies mode to v1 first and hoists `move a1,s1; move a2,s2; move a3,t0` (the eb74 arguments, 4 args incl. c) into the
// store block before the first compare (beq v1,2); ours keeps them in the later blocks and compares a3 directly. eb74 needs the 4th argument (59 words without it... 3-arg version 51 differ).
// Tried: switch (59 words), if chain (this).
// near miss from lane api-lbp-b-20261121 (unreviewed)
