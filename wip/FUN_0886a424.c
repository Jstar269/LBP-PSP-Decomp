/* LBPMATCH module=EBOOT.BIN start=0x0886a424 end=0x0886a4f8 symbol=FUN_0886a424 lang=c99 cc=gcc404xs-Os */
/* Hand-written reconstruction from the disassembly (lane api-haiku-20261121, claude-haiku-5-5); exact match verified; names and types await human review. */
extern int DAT_08bef8f4;

void FUN_0884d5f4(int *out, unsigned int *src);
long long FUN_0886a2dc(char *obj, int *out);
void FUN_08ad3088(int *lock);
void FUN_08ad2810(int *lock);

int FUN_0886a424(char *param_1, unsigned int *param_2, unsigned int param_3)
{
  struct { int lo; unsigned char hi; } local_30;
  int out[6];
  long long uVar5;
  unsigned char cVar1;
  int *local_44;
  int iVar4;

  FUN_0884d5f4(out, param_2);
  out[3] = param_3;
  uVar5 = FUN_0886a2dc(param_1 + 200, out);
  cVar1 = ((unsigned char *)&uVar5)[4];
  local_30.lo = (int)uVar5;
  local_30.hi = cVar1;
  local_44 = (int *)out[1];
  if (local_44 != 0) {
    FUN_08ad3088(&DAT_08bef8f4);
    iVar4 = local_44[1];
    local_44[1] = iVar4 - 1;
    FUN_08ad2810(&DAT_08bef8f4);
    if (iVar4 - 1 == 0) {
      int *vt = (int *)local_44[0];
      ((void (*)(int *))vt[2])(local_44);
    }
  }
  if (cVar1 != 0) {
    *(int *)(param_1 + 0x1c8) = *(int *)(param_1 + 0x1c8) + 1;
  }
  return 1;
}
// NEAR: lbpmatch 42 words differ (51/53 compared words). Still differs: frame size (ours 64, retail 80) and the
// stack placement of the 8-byte return value: retail keeps uVar5 at sp+32 (lw/sw v0,v1 then lbu sp+36 for cVar1)
// and a separate 8-byte local at sp+0 (sw v0,0(sp); sb v1,4(sp), kept although nothing reads it).
// Ruled out: out[4] and out[6] (same frame), `&uVar5` byte read (matches cVar1 via memory), local_30 declared
// first (unused store removed).
// out = sp+8, out[3] = param_3 (sw s1,20(sp)), out[1] = local_44 (sp+12): these match retail.
// near miss from lane api-haiku-20261121 (unreviewed)
