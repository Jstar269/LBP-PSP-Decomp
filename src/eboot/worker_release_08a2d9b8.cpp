/* LBPMATCH module=EBOOT.BIN start=0x08a2d9b8 end=0x08a2da88 symbol=FUN_08a2d9b8 lang=c++ cc=gcc404xs-O2 */
/* Hand-written reconstruction from the disassembly (lane lbp19o, Claude Opus 5.5): scoped-lock inline members; matching idiom: the ready flag is `volatile` (retail keeps lbu + andi 0xff for each read under the lock); exact match verified; names and types await human review. */
struct Mutex_08a2d9b8 { int m; };
extern "C" void FUN_08ad3088(Mutex_08a2d9b8 *m);
extern "C" void FUN_08ad2810(Mutex_08a2d9b8 *m);
extern "C" int FUN_08ad3098(int us);
struct Sub_08a2d9b8 { int s; };
extern "C" void FUN_08ad2100(Sub_08a2d9b8 *s);
struct ScopedLock_08a2d9b8 {
  Mutex_08a2d9b8 *m;
  ScopedLock_08a2d9b8(Mutex_08a2d9b8 *mm) : m(mm) { FUN_08ad3088(m); }
  ~ScopedLock_08a2d9b8() { FUN_08ad2810(m); }
};
struct App_08a2d9b8 { char pad[0x196]; bool b196; bool b197; bool b198; bool b199; };
extern "C" App_08a2d9b8 *FUN_089ecaa8();
struct P_08a2d9b8 {
  char pad0[8];
  Sub_08a2d9b8 sub;               /* +0x8 */
  char pad[0x1ac - 0x8 - sizeof(Sub_08a2d9b8)];
  Mutex_08a2d9b8 lock;            /* +0x1ac */
  char pad2[0x248 - 0x1b0];
  unsigned char count;   /* +0x248 */
  volatile bool ready;   /* +0x249 */
  bool IsReady() { ScopedLock_08a2d9b8 l(&lock); return ready; }
  void Request() { ScopedLock_08a2d9b8 l(&lock); ++count; }
  void Shutdown() { ScopedLock_08a2d9b8 l(&lock); FUN_08ad2100(&sub); }
  void Release()
  {
    ScopedLock_08a2d9b8 l(&lock);
    if (count != 0) {
      if (--count == 0)
        Shutdown();
    }
  }
};
extern P_08a2d9b8 *DAT_08bee648;
extern "C" void FUN_08a2d9b8()
{
  P_08a2d9b8 *p = DAT_08bee648;
  if (!p->IsReady())
    return;
  App_08a2d9b8 *a = FUN_089ecaa8();
  a->b196 = false;
  a->b199 = false;
  a->b198 = false;
  a->b197 = true;
  p->Release();
}
