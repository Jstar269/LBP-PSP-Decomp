/* LBPMATCH module=EBOOT.BIN start=0x089eba28 end=0x089ebb2c symbol=FUN_089eba28 lang=c++ cc=gcc404xs-O2 */
/* Hand-written reconstruction from the disassembly (lbp-decomp lane 2026-11-04, Claude Opus 5.5); exact match verified; names and types await human review; matching idiom: get() declared weak and left undefined (the only exact form found: retail inlines a weak get(), FUN_08804f34, whose address the safe-bool test keeps; lbpmatch places weak data only, so an inline definition here would be compiled into this object at a wrong address). */
struct Thing_089eba28 { int pad; int kind; };
struct Session_089eba28 { int handle; };
class Ref_08804f3c {
public:
  typedef Thing_089eba28 *(Ref_08804f3c::*Bool)() const;
  ~Ref_08804f3c();
  Thing_089eba28 *get() const __attribute__((weak));
  operator Bool() const { return ptr ? &Ref_08804f3c::get : 0; }
  Thing_089eba28 *operator->() const { return ptr; }
private:
  Thing_089eba28 *ptr;
  int *count;
};
struct App_089eba28 { int pad[0x9c / 4]; Session_089eba28 session; };
extern "C" int DAT_08bf92f4;
extern "C" Ref_08804f3c FUN_088b4a5c(Session_089eba28 *session);
extern "C" void *FUN_08a9e838(unsigned size);
extern "C" void FUN_088b1108(void *obj);
extern "C" Ref_08804f3c FUN_0883bd5c(void *obj);
extern "C" int FUN_088b3e28(Session_089eba28 *session, const Ref_08804f3c &cur, const Ref_08804f3c &next);
extern "C" int FUN_089eba28(App_089eba28 *app)
{
  Ref_08804f3c ref = FUN_088b4a5c(&app->session);
  int kind = ref ? ref->kind : 0;
  if (DAT_08bf92f4 != 0)
    return 0;
  if (kind == 0 || kind == 16 || kind == 15 || kind == 8)
    return 0;
  void *obj = FUN_08a9e838(20);
  FUN_088b1108(obj);
  return FUN_088b3e28(&app->session, ref, FUN_0883bd5c(obj));
}
