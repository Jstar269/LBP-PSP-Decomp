/* LBPMATCH module=EBOOT.BIN start=0x08913990 end=0x089139b4 symbol=_ZN12Obj_08913990C1Ev lang=c++ */
/* Hand-written reconstruction from the disassembly (lane lbp19o, Claude Opus 5.5); exact match verified; names and types await human review. */
struct ListNode_08913990 {
  ListNode_08913990 *next;
  ListNode_08913990 *prev;
  ListNode_08913990() : next(this), prev(this) {}
};

struct Obj_08913990 {
  bool field_00;
  ListNode_08913990 head;
  int field_0c;
  int field_10;
  bool field_14;
  Obj_08913990();
};

Obj_08913990::Obj_08913990() : field_00(false), field_0c(0), field_10(0x400), field_14(false) {}
