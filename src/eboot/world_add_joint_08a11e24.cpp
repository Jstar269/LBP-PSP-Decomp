/* LBPMATCH module=EBOOT.BIN start=0x08a11e24 end=0x08a11ec8 symbol=FUN_08a11e24 lang=c++ cc=gcc404xs-O2 */
/* Hand-written reconstruction from the disassembly (lane lbp19o, Claude Opus 5.5): a std::vector-style push_back with an iterator class whose end() is built from a reference to the finish pointer; exact match verified; names and types await human review. */
inline void *operator new(unsigned int, void *p) throw() { return p; }

struct Joint_08a11e24 {
  char pad[0x5c];
  int type;  /* +0x5c */
};

struct JointIter_08a11e24 {
  Joint_08a11e24 **cur;
  JointIter_08a11e24() : cur(0) {}
  explicit JointIter_08a11e24(Joint_08a11e24 **const &p) : cur(p) {}
  Joint_08a11e24 *&operator*() const { return *cur; }
  JointIter_08a11e24 &operator++() { ++cur; return *this; }
};
inline bool operator==(const JointIter_08a11e24 &a, const JointIter_08a11e24 &b) { return a.cur == b.cur; }
inline bool operator!=(const JointIter_08a11e24 &a, const JointIter_08a11e24 &b) { return a.cur != b.cur; }

struct JointVec_08a11e24 {
  Joint_08a11e24 **start;
  Joint_08a11e24 **finish;
  Joint_08a11e24 **end_of_storage;
  JointIter_08a11e24 begin() { return JointIter_08a11e24(start); }
  JointIter_08a11e24 end() { return JointIter_08a11e24(finish); }
  void insert_aux(JointIter_08a11e24 pos, Joint_08a11e24 *const &x);
  void push_back(Joint_08a11e24 *const &x)
  {
    if (finish != end_of_storage) {
      ::new (static_cast<void *>(finish)) Joint_08a11e24 *(x);
      ++finish;
    } else {
      insert_aux(end(), x);
    }
  }
};

struct World_08a11e24 {
  char pad[0x2014];
  JointVec_08a11e24 joints[4];  /* +0x2014 */
};

extern "C" void FUN_08a11e24(World_08a11e24 *world, Joint_08a11e24 *joint)
{
  JointVec_08a11e24 &v = world->joints[joint->type];
  for (JointIter_08a11e24 it = v.begin(); it != v.end(); ++it) {
    if (*it == joint)
      return;
  }
  v.push_back(joint);
}
