/* LBPMATCH module=EBOOT.BIN start=0x08867064 end=0x08867164 symbol=FUN_08867064 lang=c99 cc=gcc404xs-Os */
/* Hand-written reconstruction from the disassembly (lane api-haiku-20261121, claude-haiku-5-5); exact match verified; names and types await human review. */
extern int DAT_08bef8f4;
extern int DAT_08bedba4;

void FUN_08866d9c(int **out, char *self);
void FUN_08866a7c(char *a, char *b, int **c, int d);
void FUN_08ad3088(int *lock);
void FUN_08ad2810(int *lock);
void FUN_0887c328(int a, char *b);
void FUN_08866590(char *self);

int FUN_08867064(char *param_1)
{
  int *local_30[4];
  int *obj;
  int iVar;
  int *p;

  FUN_08866d9c(local_30, param_1);
  p = (int *)(param_1 + 0x12c);
  FUN_08866a7c((char *)p, param_1 + 0x114, local_30, *(int *)(param_1 + 0x128));
  obj = local_30[0];
  if (obj != 0) {
    FUN_08ad3088(&DAT_08bef8f4);
    iVar = obj[1] - 1;
    obj[1] = iVar;
    FUN_08ad2810(&DAT_08bef8f4);
    if (iVar == 0) {
      int *vt = (int *)obj[0];
      ((void (*)(int *))vt[1])(obj);
    }
  }
  FUN_0887c328(DAT_08bedba4, (char *)p);
  if (p[1] == 6) {
    FUN_08866590(param_1);
    param_1[0x244] = 1;
  }
  else {
    int *q = (int *)(param_1 + 0x140);
    if (q[0] == 2 && q[1] == 0x194) {
      param_1[0x244] = 1;
    }
  }
  return 1;
}
// 15 of 64 words differ (lbpmatch 65/64; prologue, call sequence, refcount release and the first checks match).
// Remaining difference is the tail: retail's bne delay slot holds `li v0,1` (the sb value) where ours holds `lw ra`;
// the if/else, goto, and flag forms of the 0x130/0x140 tests all gave the same layout.
// Ruled out: p-relative vs s2-relative field offsets (fixed: int *p = param+0x12c), the goto done form.
// near miss from lane api-haiku-20261121 (unreviewed)
