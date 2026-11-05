/* LBPMATCH module=EBOOT.BIN start=0x089eb6a8 end=0x089eb734 symbol=FUN_089eb6a8 lang=c++ cc=gcc404xs-O2 */
/* Hand-written reconstruction from the disassembly (lbp-decomp lane 2026-11-04, Claude Opus 5.5); exact match verified; names and types await human review; matching idiom: get() declared weak and left undefined (the only exact form found: retail inlines a weak get(), FUN_08804f34, whose address the safe-bool test keeps; lbpmatch places weak data only, so an inline definition here would be compiled into this object at a wrong address). */
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
  int pad[0x9c / 4];
  Session session; /* +0x9c */
};
extern "C" Ref_08804f3c FUN_088b4a5c(Session *session);
extern "C" int FUN_088b3974(Session *session, const Ref_08804f3c &ref);

extern "C" int FUN_089eb6a8(App *app)
{
  Ref_08804f3c ref = FUN_088b4a5c(&app->session);
  if (ref && ref->kind == 8)
    return FUN_088b3974(&app->session, ref);
  return 0;
}
