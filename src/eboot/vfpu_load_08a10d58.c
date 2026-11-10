/* LBPMATCH module=EBOOT.BIN start=0x08a10d58 end=0x08a10d60 symbol=FUN_08a10d58 lang=c99 asm=vfpu */
/* Hand-written reconstruction from the disassembly (lbp-decomp supervisor turn 2026-11-09, Claude Opus 5.5); exact match verified under build psp-gcc-4.0.4c (the lv.q fills the jr ra delay slot); names and types await human review. */
/* Original inline VFPU asm: the function only loads a quad into VFPU column C000, which no C construct of this compiler produces. */
struct S_08a10d58 { char pad[32]; float v[4]; };
void FUN_08a10d58(struct S_08a10d58 *p)
{
    __asm__("lv.q C000, %0" : : "m"(p->v[0]));
}
