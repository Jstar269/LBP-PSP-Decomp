/* LBPMATCH module=EBOOT.BIN start=0x0897917c end=0x089791a4 symbol=FUN_0897917c lang=c++ cc=gcc404xs-O1 */
/* Hand-written reconstruction from the disassembly (lane lbp4d, Claude Sonnet 5.5); exact only under the unlocked profile gcc404xs-O1 (parked 2026-11-10: not promotable until -O1 is locked for this unit or a locked profile matches; lbpsweep re-tests it); names and types await human review. Calls the fourth virtual function of the object at +0x10; cc=gcc404xs-O1 because retail makes no sibling call. */
class Object {
public:
    virtual void m0();
    virtual void m1();
    virtual void m2();
    virtual void m3();
};

struct Owner {
    char pad[0x10];
    Object *obj;
};

extern "C" void FUN_0897917c(Owner *owner)
{
    owner->obj->m3();
}
// lane lbp4d: EXACT in the lane with cc=gcc404xs-O1; refused by the promotion gate because the toolchain lock does
// not record an -O1 profile. Promote after lbppromote.py migrate-toolchain adds it (if more -O1 units turn up).
