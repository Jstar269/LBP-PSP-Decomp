/* LBPMATCH module=EBOOT.BIN start=0x088b61bc end=0x088b6260 symbol=FUN_088b61bc lang=c++ cc=gcc404xs-Os */
/* Hand-written reconstruction from the disassembly (lbp-decomp lane 2026-11-04, Claude Opus 5.5); exact match verified; names and types await human review. */
struct GeList_088b61bc { unsigned start; unsigned *cur; };
extern "C" GeList_088b61bc *DAT_08bdc188;
struct Surface_088b61bc { int f0; int width; int height; };
struct Layer_088b61bc { char pad[168]; Surface_088b61bc *surface; };
struct Screen_088b61bc { char pad[196]; Layer_088b61bc *layer; };
extern "C" void FUN_08a24424(Surface_088b61bc *s, GeList_088b61bc *l);
extern "C" void FUN_08a23f64(int x, int y, int w, int h, int u, int v, int tw, int th, int color);
extern "C" void FUN_088b61bc(Screen_088b61bc *s)
{
  if (s->layer == 0)
    return;
  Surface_088b61bc *surf = s->layer->surface;
  FUN_08a24424(surf, DAT_08bdc188);
  GeList_088b61bc *l = DAT_08bdc188;
  unsigned *c = l->cur;
  c[0] = 0x21000001;
  c[1] = 0xdf000036;
  c[2] = 0xe0000000;
  c[3] = 0xe1000000;
  l->cur = c + 4;
  FUN_08a23f64(0, 0, 480, 272, 0, 0, surf->width, surf->height, 0xffff);
}
