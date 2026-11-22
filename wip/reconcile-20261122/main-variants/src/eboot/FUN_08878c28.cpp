/* LBPMATCH module=EBOOT.BIN start=0x08878c28 end=0x08878cb8 symbol=FUN_08878c28 lang=c++ */
/* Hand-written reconstruction from the disassembly (lane lbp17a, Codex GPT-6); exact match verified; names and types await human review. */
typedef unsigned int u32;
struct Self {
  char prefix[16];
  u32 index;
};

extern "C" void FUN_08a9e0c4(void *);
extern "C" void *FUN_08a9e824(u32);

extern "C" void *FUN_08878c28(Self *self, u32 requested)
{
  u32 index = self->index;
  u32 *entry = (u32 *)((index << 2) + (u32)self);
  u32 capacity = entry[2];

  if (capacity < requested) {
    void *old = (void *)entry[0];
    if (old != 0) {
      FUN_08a9e0c4(old);
    }
    index = self->index;
    void *allocated = FUN_08a9e824(requested);
    capacity = requested;
    entry = (u32 *)((index << 2) + (u32)self);
    entry[0] = (u32)allocated;
  }
  index = self->index;
  entry = (u32 *)((index << 2) + (u32)self);
  entry[2] = capacity;
  return (void *)entry[0];
}
