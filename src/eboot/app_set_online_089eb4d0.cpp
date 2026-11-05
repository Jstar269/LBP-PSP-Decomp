/* LBPMATCH module=EBOOT.BIN start=0x089eb4d0 end=0x089eb5a4 symbol=FUN_089eb4d0 lang=c++ cc=gcc404xs-O2 */
/* Hand-written reconstruction from the disassembly (lbp-decomp lane 2026-11-04, Claude Opus 5.5); exact match verified; names and types await human review; matching idioms: volatile App flags, as the App running flag in 0x088048a8 (the only exact form found), and get() declared weak and left undefined (retail inlines a weak get(), FUN_08804f34, whose address the safe-bool test keeps; lbpmatch places weak data only). */
struct Thing {
  int pad;
  int kind; /* +0x4 */
};
struct Session {
  int handle;
};
class Ref_08804f3c {
public:
  typedef Thing *(Ref_08804f3c::*Bool)() const;
  ~Ref_08804f3c();
  Thing *get() const __attribute__((weak));
  operator Bool() const { return ptr ? &Ref_08804f3c::get : 0; }
  Thing *operator->() const { return ptr; }
private:
  Thing *ptr;
  int *count;
};
struct App {
  volatile bool running;
  volatile bool online;  /* +0x1 */
  int pad[(0x9c - 4) / 4];
  Session session;       /* +0x9c */
};
struct Presence;
extern "C" Ref_08804f3c FUN_088b4a5c(Session *session);
extern "C" bool FUN_08a50fbc(void);
extern "C" Presence *DAT_08bf7304;
extern "C" void FUN_08a7e774(Presence *presence, bool online);
extern "C" void *FUN_08914a7c(void);
extern "C" void FUN_08914384(void *p);
extern "C" void FUN_08914404(void *p);

extern "C" void FUN_089eb4d0(App *app, bool online)
{
  Ref_08804f3c ref = FUN_088b4a5c(&app->session);
  if (ref && ref->kind == 1) {
    if (app->online == online)
      return;
    app->online = online;
    if (FUN_08a50fbc() && DAT_08bf7304)
      FUN_08a7e774(DAT_08bf7304, online);
    if (online)
      FUN_08914384(FUN_08914a7c());
    else
      FUN_08914404(FUN_08914a7c());
  }
}
