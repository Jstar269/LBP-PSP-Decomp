/* LBPMATCH module=EBOOT.BIN start=0x08a0b448 end=0x08a0b480 symbol=FUN_08a0b448 lang=c++ cc=gcc404xs-O2 */
/* Hand-written reconstruction from the disassembly (lbp-decomp lane 2026-11-04, Claude Opus 5.5); exact match verified; names and types await human review. */
struct Motor {
  virtual void v0();
  virtual void v1();
  virtual void v2();
  virtual void v3();
  virtual void v4();
  virtual void v5();
  virtual void v6();
  virtual void v7();
  virtual void v8();
  virtual void v9();
  virtual void v10();
  virtual void v11();
  virtual void v12();
  virtual void v13();
  virtual void v14();
  virtual void v15();
  virtual void v16();
  virtual void v17();
  virtual void v18();
  virtual void v19();
  virtual void v20();
  virtual void v21();
  virtual void v22();
  virtual void v23();
  virtual void v24();
  virtual void v25();
  virtual void v26();
  virtual void v27();
  virtual float Span();
  int pad[(0x80 - 4) / 4];
  float start; /* +0x80 */
  int pad2[(0xa0 - 0x84) / 4];
  float pos;   /* +0xa0 */
};

extern "C" float FUN_08a0b448(Motor *motor)
{
  return (motor->pos - motor->start) / motor->Span();
}
