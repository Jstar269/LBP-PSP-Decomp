/* LBPMATCH module=EBOOT.BIN start=0x08a9ac50 end=0x08a9acd0 symbol=FUN_08a9ac50 lang=c++ */
/* Hand-written reconstruction from the disassembly (lbp-decomp lane 2026-11-04, Claude Opus 5.5); exact match verified; names and types await human review. */
struct Listener {
  virtual void v0();
  virtual void v1();
  virtual void Tick();
};
struct ListenerSet {
  Listener **begin;
  Listener **end;
  Listener **capacity;
  int mutex; /* +0xc */
};
extern "C" ListenerSet *DAT_08bf7dd8;
extern "C" int sceKernelLockMutex(int mutex, int count, unsigned int *timeout);
extern "C" int sceKernelUnlockMutex(int mutex, int count);

extern "C" void FUN_08a9ac50(void)
{
  if (!DAT_08bf7dd8)
    return;
  sceKernelLockMutex(DAT_08bf7dd8->mutex, 1, 0);
  for (Listener **it = DAT_08bf7dd8->begin; it != DAT_08bf7dd8->end; ++it)
    (*it)->Tick();
  sceKernelUnlockMutex(DAT_08bf7dd8->mutex, 1);
}
