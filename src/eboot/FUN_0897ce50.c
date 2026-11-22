/* LBPMATCH module=EBOOT.BIN start=0x0897ce50 end=0x0897cf2c symbol=FUN_0897ce50 lang=c99 */
/* Hand-written reconstruction from the disassembly (lane api-lbp-b-20261121, claude-sonnet-5-5); exact match verified; names and types await human review. */
struct Ref { int a; int b; };
struct Tmp { int a; int b; struct Ref r; };
struct Self {
  int pad0;
  int f4;
  char pad1[4];
  unsigned char on;
  char pad2[15];
  int f28;
  char pad3[8];
  int f40;
  char pad4[8];
  int f52;
  char pad5[36];
  int f92;
  char pad6[4];
  int f100;
};
extern char DAT_08bbd9cc[];
extern char DAT_08bb3758[];
extern int *FUN_089f683c(void);
extern void FUN_088399bc(int a, int b, char *fmt, int x);
extern void FUN_0897633c(struct Tmp *t, int s);
extern void FUN_089d1dfc(int o, struct Tmp *t, int z);
extern void FUN_088cc71c(struct Ref *r);
extern void FUN_089d222c(int o, int z);

void FUN_0897ce50(struct Self *self)
{
  struct Tmp t1;
  struct Tmp t2;
  if (self->on != 0) {
    int a = self->f100;
    int *p = FUN_089f683c();
    FUN_088399bc(a, 8, DAT_08bbd9cc, *p);
    {
      int o = self->f92;
      FUN_0897633c(&t2, self->f100);
      FUN_089d1dfc(o, &t2, 0);
    }
    FUN_088cc71c(&t2.r);
    FUN_089d222c(self->f92, 0);
  }
  FUN_088399bc(self->f52, 16, DAT_08bb3758, self->f4);
  {
    int o = self->f40;
    FUN_0897633c(&t1, self->f52);
    FUN_089d1dfc(o, &t1, 0);
  }
  FUN_088cc71c(&t1.r);
  FUN_089d222c(self->f40, 0);
}
