/* LBPMATCH module=EBOOT.BIN start=0x08a26bec end=0x08a26cac symbol=FUN_08a26bec lang=c99 cc=gcc404xs-O2 */
/* Hand-written reconstruction from the disassembly (lane lbp3a, Claude Sonnet 5.5); exact match verified under cc=gcc404xs-O2 after a compiler-profile sweep (lane lbp20s, Claude Opus 5.5); names and types await human review. */
typedef unsigned char uchar;
typedef unsigned int undefined4;

extern void FUN_08a1dd1c();
extern void FUN_08a26210();
extern void FUN_08a26784();

struct Sub { char pad[0x28]; int count; };
struct Obj;
struct Vt { void *pad[3]; struct Sub *(*get)(struct Obj *); };
struct Obj { struct Vt *vt; };

void FUN_08a26bec(struct Obj **param_1,int *param_2,int *param_3,uchar param_4)
{
  struct Sub *s;

  if (param_3 != (int *)0x0) {
    FUN_08a1dd1c(param_3);
  }
  s = (*param_1)->vt->get(*param_1);
  if (0 < s->count) {
    FUN_08a26210(param_1,param_2,param_3);
    return;
  }
  s = (*param_1)->vt->get(*param_1);
  FUN_08a26784(s,param_2,param_3,param_4);
  return;
}
