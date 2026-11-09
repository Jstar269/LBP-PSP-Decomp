/* LBPMATCH module=EBOOT.BIN start=0x08a239e8 end=0x08a23adc symbol=FUN_08a239e8 lang=c++ cc=gcc404xs-O2 */
/* Hand-written reconstruction from the disassembly (lane lbp19o, Claude Opus 5.5); the buffer toggle precedes the index update in the source (sched1 hoists the index work above it); exact match verified; names and types await human review. */
extern "C" int sceDisplaySetFrameBuf(void *top, int bw, int fmt, int sync);
extern "C" int sceDisplayWaitVblankStart(void);
extern "C" int sceDisplayGetVcount(void);
struct Buf { int fmt; int f4; char *addr; };
extern Buf DAT_08bf5ee8[];
extern int DAT_08bf6074, DAT_08bf6078, DAT_08bf6070;
extern Buf *DAT_08bdc180, *DAT_08bdc184;
extern "C" void FUN_08a239e8(bool keep)
{
  DAT_08bdc180 = DAT_08bdc184;
  if (!keep)
    DAT_08bf6078 = 1 - DAT_08bf6078;
  DAT_08bf6074 = 1 - DAT_08bf6074;
  DAT_08bdc184 = &DAT_08bf5ee8[DAT_08bf6074];
  sceDisplaySetFrameBuf(DAT_08bdc184->addr + 0x4000000, 512, DAT_08bdc184->fmt, 1);
  sceDisplayWaitVblankStart();
  if (sceDisplayGetVcount() - DAT_08bf6070 == 1)
    sceDisplayWaitVblankStart();
  DAT_08bf6070 = sceDisplayGetVcount();
}
