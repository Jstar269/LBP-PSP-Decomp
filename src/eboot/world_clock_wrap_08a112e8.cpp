/* LBPMATCH module=EBOOT.BIN start=0x08a112e8 end=0x08a1139c symbol=FUN_08a112e8 lang=c++ cc=gcc404xs-O2 rodata=0x08bc3650 */
/* Hand-written reconstruction from the disassembly (lbp-decomp lane 2026-11-04, Claude Opus 5.5); exact match verified; names and types await human review. */
struct Motor;
struct Joint {
  virtual void v0();
  virtual void v1();
  virtual void v2();
  virtual void v3();
  virtual void v4();
  virtual void v5();
  virtual void v6();
  virtual void v7();
  virtual void v8();
  virtual Motor *GetMotor();
};
struct JointList {
  int kind;
  Joint **begin;
  Joint **end;
};
struct World {
  char pad[0x2010];
  JointList joints[4]; /* +0x2010 */
  char pad2[0x206c - 0x2040];
  float clock;         /* +0x206c */
};
extern "C" void FUN_08a0d758(Motor *motor, float seconds);

extern "C" void FUN_08a112e8(World *world)
{
  for (int i = 0; i < 4; ++i) {
    JointList &list = world->joints[i];
    for (Joint **it = list.begin; it != list.end; ++it)
      if (Motor *motor = (*it)->GetMotor())
        FUN_08a0d758(motor, 3600.0f);
  }
  world->clock -= 3600.0f;
}
