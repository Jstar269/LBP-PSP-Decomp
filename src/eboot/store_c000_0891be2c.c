/* LBPMATCH module=EBOOT.BIN start=0x0891be2c end=0x0891be34 symbol=FUN_0891be2c lang=c99 asm=vfpu */
/* Hand-written reconstruction from the disassembly (lane lbp20f, Claude Opus 5.5); exact match verified; names and types await human review. */
/* Original inline VFPU asm: the function stores VFPU column C000, which no instruction of the function writes; only
   inline asm hands VFPU register contents from one statement (in the caller) to the next. */
struct V_0891be2c {
    char pad[16];
    float v[4];
};

void FUN_0891be2c(struct V_0891be2c *p)
{
    __asm__("sv.q C000, %0" : "=m"(p->v[0]));
}
