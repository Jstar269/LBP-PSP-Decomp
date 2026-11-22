/* LBPMATCH module=EBOOT.BIN start=0x089757c0 end=0x08975850 symbol=FUN_089757c0 lang=c99 */
/* Hand-written reconstruction from the disassembly (lane api-lbp-b-20261121, claude-sonnet-5-5); exact match verified; names and types await human review. */
struct Rec { int a; int *p; int c; int d; };
extern struct Rec DAT_08bbd338[];
extern void *FUN_08914a7c(void);
extern int FUN_08914e94(void *o, int v);
extern void FUN_08913dd8(void *o, int a, int b, int c, int d, int e, int f, int g);

void FUN_089757c0(int idx)
{
  struct Rec *r = &DAT_08bbd338[idx];
  if (idx != 1) {
    void *o1 = FUN_08914a7c();
    void *o2 = FUN_08914a7c();
    int v = FUN_08914e94(o2, *r->p);
    FUN_08913dd8(o1, 0, v, r->c, 0, 0, r->d, 0);
  }
}
