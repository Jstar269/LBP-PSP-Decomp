/* LBPMATCH module=EBOOT.BIN start=0x088596cc end=0x0885979c symbol=FUN_088596cc lang=c++ */
/* Hand-written reconstruction from the disassembly (lane lbp17g, GPT-6); exact match verified; names and types await human review. */
struct Obj {
  virtual void v0();
  virtual void destroy();
  int references;
};
struct Source { char pad[12]; void *inner; };
struct StringObj;
extern "C" int DAT_08bef8f4;
extern "C" void FUN_0885cacc(Obj **, Source *);
extern "C" void *FUN_08875f58(Obj *, int *);
extern "C" void FUN_08ad3088(void *);
extern "C" void FUN_08ad2810(void *);
extern "C" void *FUN_08a9e838(unsigned);
extern "C" void FUN_088770b8(StringObj *, unsigned char *);
extern "C" int *FUN_088596cc(int *out, Source *source)
{
  Obj *o;
  int key[3];
  key[0] = 0;
  FUN_0885cacc(&o, source);
  void *value = FUN_08875f58(o, key);
  Obj *p = o;
  if (p) {
    FUN_08ad3088(&DAT_08bef8f4);
    int count = p->references - 1;
    p->references = count;
    FUN_08ad2810(&DAT_08bef8f4);
    if (count == 0)
      p->destroy();
  }
  if (value) {
    StringObj *created = (StringObj *)FUN_08a9e838(16);
    FUN_088770b8(created, (unsigned char *)value);
    *out = (int)created;
  } else {
    *out = 0;
  }
  return out;
}
