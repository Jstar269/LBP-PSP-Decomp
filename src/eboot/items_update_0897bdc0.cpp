/* LBPMATCH module=EBOOT.BIN start=0x0897bdc0 end=0x0897be1c symbol=FUN_0897bdc0 lang=c++ */
/* Hand-written reconstruction from the disassembly (lbp-decomp lane 2026-11-04, Claude Opus 5.5); exact match verified; names and types await human review. */
struct Item {
  virtual void v0();
  virtual void v1();
  virtual void Update(float dt);
};
struct ItemList {
  Item **begin;
  Item **end;
};

extern "C" void FUN_0897bdc0(ItemList *list, float dt)
{
  for (Item **it = list->begin; it != list->end; ++it)
    (*it)->Update(dt);
}
