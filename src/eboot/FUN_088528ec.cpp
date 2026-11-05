/* LBPMATCH module=EBOOT.BIN start=0x088528ec end=0x0885297c symbol=FUN_088528ec lang=c++ */
/* Hand-written reconstruction from the disassembly (lane lbp-decomp-b, Claude Sonnet 5.5); exact match verified; names and types await human review. */
extern "C" void FUN_0894a258(void *p);
extern "C" void FUN_08895dc0(void *p);
extern void *DAT_08baedc0[], *DAT_08baed98[], *DAT_08baedac[];
struct L { L *next; L *prev; };
struct Obj { void **vt; int f4; void **f8; int pad; int pad2; L l20; L l28; int f36, f40, f44, f48, f52, f56; int pad3[3]; short s72; };
extern "C" void FUN_088528ec(Obj *o)
{
  FUN_0894a258(o);
  o->vt = DAT_08baedc0;
  o->f4 = 1;
  FUN_0894a258(&o->f8);
  o->vt = DAT_08baed98;
  L *h1 = &o->l20;
  L *h2 = &o->l28;
  o->f8 = DAT_08baedac;
  h1->next = h1;
  h1->prev = h1;
  h2->next = h2;
  h2->prev = h2;
  o->f36 = 0;
  o->f40 = 0;
  o->f44 = 0;
  o->f48 = 0;
  o->f52 = 0;
  o->f56 = 0;
  o->s72 = 0;
  FUN_08895dc0(o);
}
