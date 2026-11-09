/* LBPMATCH module=EBOOT.BIN start=0x08a11108 end=0x08a111ec symbol=FUN_08a11108 lang=c++ cc=gcc404xs-O2 */
/* Hand-written reconstruction from the disassembly (lane lbp19o, Claude Opus 5.5): bool bitfields tested with `|`, iterating four joint vectors through an iterator class; exact match verified; names and types await human review. */
struct Body_08a11108 { unsigned int flags; };
struct Joint_08a11108 {
  virtual void v0(); virtual void v1(); virtual void v2(); virtual void v3(); virtual void v4(); virtual void v5();
  virtual void v6(); virtual void v7(); virtual void v8(); virtual void v9(); virtual void v10(); virtual void v11();
  virtual void v12(); virtual void v13(); virtual void v14(); virtual void v15(); virtual void v16(); virtual void v17();
  virtual void v18(); virtual void v19(); virtual void v20(); virtual void v21(); virtual void v22(); virtual void v23();
  virtual void v24(); virtual void v25(); virtual void v26(); virtual void v27(); virtual void v28(); virtual void v29();
  virtual void v30(); virtual void vfunc_7c();
  Body_08a11108 *a;   /* +0x4 */
  Body_08a11108 *b;   /* +0x8 */
  char pad[0x64 - 0xc];
  bool f0 : 1; bool f1 : 1; bool b2 : 1; bool f3 : 1;
  bool b4 : 1; bool b5 : 1; unsigned int f6 : 3; bool b9 : 1; unsigned int rest : 22;
};
struct JointIter_08a11108 {
  Joint_08a11108 **cur;
  explicit JointIter_08a11108(Joint_08a11108 **const &p) : cur(p) {}
  Joint_08a11108 *&operator*() const { return *cur; }
  JointIter_08a11108 &operator++() { ++cur; return *this; }
  JointIter_08a11108 operator++(int) { JointIter_08a11108 t(*this); ++cur; return t; }
};
inline bool operator!=(const JointIter_08a11108 &a, const JointIter_08a11108 &b) { return a.cur != b.cur; }
struct JointList_08a11108 {
  Joint_08a11108 **start; Joint_08a11108 **finish; Joint_08a11108 **eos;
  JointIter_08a11108 begin() { return JointIter_08a11108(start); }
  JointIter_08a11108 end() { return JointIter_08a11108(finish); }
};
struct World_08a11108 {
  char pad[0x2014];
  JointList_08a11108 joints[4];   /* +0x2014 */
  char pad2[0x2070 - 0x2044];
  bool disabled;         /* +0x2070 */
};

extern "C" void FUN_08a11108(World_08a11108 *world)
{
  if (world->disabled)
    return;
  for (int i = 0; i < 4; i++) {
    JointList_08a11108 &l = world->joints[i];
    for (JointIter_08a11108 it = l.begin(); it != l.end(); ++it) {
      Joint_08a11108 *j = *it;
      if (j->b4 | j->b5)
        continue;
      if ((j->a->flags | j->b->flags) & 0x100)
        j->b2 = 0;
      if (!j->b2 && j->b9)
        j->vfunc_7c();
    }
  }
}
