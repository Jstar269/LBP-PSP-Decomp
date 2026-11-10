/* LBPMATCH module=EBOOT.BIN start=0x08a41b78 end=0x08a41b80 symbol=FUN_08a41b78 lang=c99 asm=vfpu */
/* Hand-written reconstruction from the disassembly (lbp-decomp supervisor turn 2026-11-09, Claude Opus 5.5); exact match verified under build psp-gcc-4.0.4c (the lv.q fills the jr ra delay slot); names and types await human review. */
/* Original inline VFPU asm: the function only loads a quad into VFPU column C000, which no C construct of this compiler produces. */
struct S_08a41b78 { char pad[272]; float v[4]; };
void FUN_08a41b78(struct S_08a41b78 *p)
{
    __asm__("lv.q C000, %0" : : "m"(p->v[0]));
}
