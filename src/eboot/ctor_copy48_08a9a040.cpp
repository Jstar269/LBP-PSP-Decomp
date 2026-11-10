/* LBPMATCH module=EBOOT.BIN start=0x08a9a040 end=0x08a9a0b8 symbol=FUN_08a9a040 lang=c++ */
/* Hand-written reconstruction from the disassembly (lane lbp-decomp-b, Claude Sonnet 5.5); exact match verified; names and types await human review. */
extern "C" void FUN_08a41e50(void *o, void *a);
extern void *DAT_08bcaa38[];
struct M { int w[12]; };
struct Src { int pad[2]; M m; };
struct Obj { void **vt; int pad[105]; M m; char b472; };
extern "C" void FUN_08a9a040(Obj *o, void *a, Src *s)
{
  FUN_08a41e50(o, a);
  o->vt = DAT_08bcaa38;
  o->b472 = 0;
  o->m = s->m;
}
// lane lbp19o: 6 words left, all in the 48-byte struct-copy loop (movmem expansion): retail stores the first word,
// increments the destination by 16 and then stores the other three with offsets -12/-8/-4 (the last in the bne delay
// slot); ours increments after all four stores. Same 6 at gcc404xs-Os/-O2/-O3; gcc402t/gcc410 14, -O1 20; explicit
// Vec4 loops are worse (22-30). Looks like a block-move expansion or scheduler difference in the retail compiler.
// near miss from lane lbp19o (unreviewed)
