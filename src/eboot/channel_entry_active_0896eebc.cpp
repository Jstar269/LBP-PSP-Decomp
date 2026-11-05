/* LBPMATCH module=EBOOT.BIN start=0x0896eebc end=0x0896ef04 symbol=FUN_0896eebc lang=c++ */
/* Hand-written reconstruction from the disassembly (lbp-decomp lane 2026-11-04, Claude Opus 5.5); exact match verified; names and types await human review. */
struct Channel;
struct Voice;
struct Owner {
  char pad[0x2c];
  Channel *channels[1]; /* +0x2c */
};
extern "C" Voice *FUN_08978ef0(Channel *channel, int id);
extern "C" bool FUN_08978164(Voice *voice);

extern "C" bool FUN_0896eebc(Owner *owner, int channel, int id)
{
  Voice *voice = FUN_08978ef0(owner->channels[channel], id);
  return voice && FUN_08978164(voice);
}
