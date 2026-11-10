/* LBPMATCH module=EBOOT.BIN start=0x089ecf0c end=0x089ecf18 symbol=FUN_089ecf0c lang=c99 asm=vfpu */
/* Hand-written reconstruction from the disassembly (lbp-decomp supervisor turn 2026-11-09, Claude Opus 5.5); exact match verified under build psp-gcc-4.0.4c (the lv.q fills the jr ra delay slot); names and types await human review. */
/* Original inline VFPU asm: the function only loads a quad into VFPU column C000, which no C construct of this compiler produces. */
extern float DAT_08bf36a0[4];
void FUN_089ecf0c(void)
{
    __asm__("lv.q C000, %0" : : "m"(DAT_08bf36a0[0]));
}
