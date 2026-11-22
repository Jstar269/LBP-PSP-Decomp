/* LBPMATCH module=EBOOT.BIN start=0x0897f590 end=0x0897f684 symbol=FUN_0897f590 lang=c99 rodata=0x08bab59c */
/* Hand-written reconstruction from the disassembly (lane api-lbp-b-20261121, claude-sonnet-5-5); exact match verified; names and types await human review. */
/* The 1.8f stored to self+128 is read through DAT_08bb393c (its literal sits in another rodata block than the 1.0f). */
struct Ref { int a; int b; };
struct Tmp { int a; int b; struct Ref r; };
struct Self {
  char pad0[96];
  int f96;
  char pad1[4];
  float v104[1];
  char pad2[4];
  int f112;
  int f116;
  int f120;
  int f124;
  float f128;
};
extern const float DAT_08bb393c;
extern int DAT_08bef40c;
extern char DAT_08bbd9f4[];
extern void FUN_088399bc(int a, int b, char *fmt, int x);
extern void FUN_0897633c(struct Tmp *t, int s);
extern void FUN_089d1dfc(int o, struct Tmp *t, int z);
extern void FUN_088cc71c(struct Ref *r);
extern void FUN_089d222c(int o, int z);
extern void FUN_0897d004(void *self);
extern void FUN_0896b048(void *p, int a);
extern void FUN_0897f268(float s, void *self, int a, int b, void *v);
extern void *FUN_08914a7c(void);
extern int FUN_08914e94(void *o, int v);
extern void FUN_08913dd8(void *o, int a, int b, int c, int d, int e, int f, int g);

void FUN_0897f590(struct Self *self, int x)
{
  struct Tmp t;
  void *h1;
  void *h2;
  int v;
  FUN_088399bc(self->f124, 8, DAT_08bbd9f4, x);
  {
    int o = self->f116;
    FUN_0897633c(&t, self->f124);
    FUN_089d1dfc(o, &t, 0);
  }
  FUN_088cc71c(&t.r);
  FUN_089d222c(self->f116, 0);
  self->f128 = DAT_08bb393c;
  FUN_0897d004(self);
  FUN_0896b048((void *)self->f120, 1);
  FUN_0897f268(1.0f, self, 0, self->f96, self->v104);
  h1 = FUN_08914a7c();
  h2 = FUN_08914a7c();
  v = FUN_08914e94(h2, DAT_08bef40c);
  FUN_08913dd8(h1, 0, v, 27, 0, 0, 1024, 0);
}
