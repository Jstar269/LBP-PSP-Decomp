/* LBPMATCH module=EBOOT.BIN start=0x08a94018 end=0x08a94078 symbol=FUN_08a94018 lang=c99 */
/* Clone of src/eboot/f08a0d020_08a0d020.c (lbpclone.py tag b2): addresses substituted; exact match verified; names and types await human review. */
/* Hand-written reconstruction from the disassembly (lane lbp14b, Codex GPT-6); exact match verified; names and types await human review. */
extern char DAT_08bc32c8; extern void *FUN_08a9e838(int); extern void FUN_08a0adf8(void *); extern void FUN_08a0b5b0(void *,void *);
struct S { void *v; char pad[124]; int a,b; };
void *FUN_08a94018(void *arg) { struct S *s=FUN_08a9e838(192); FUN_08a0adf8(s); s->v=&DAT_08bc32c8; s->a=0; s->b=0; FUN_08a0b5b0(s,arg); return s; }
