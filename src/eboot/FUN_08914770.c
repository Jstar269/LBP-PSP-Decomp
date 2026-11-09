/* LBPMATCH module=EBOOT.BIN start=0x08914770 end=0x08914814 symbol=FUN_08914770 lang=c99 */
/* Hand-written reconstruction from the disassembly (lane lbp18b, GPT-6); exact match verified; names and types await human review. */
struct Owner { char pad0[0x30]; int *list; char pad1[0x1c]; unsigned char active; char pad2[3]; void *value; };
extern int FUN_08baa89c(void);
extern void FUN_08baa8d4(int);
extern void FUN_08ac6c24(int, int, int, int);
extern void FUN_08ac7ba0(void (*)(void), void (*)(void));
extern void FUN_08913e60(void);
extern void FUN_08913e7c(void);
extern void FUN_089188a0(int *);
extern int FUN_0891a58c(void);
extern int FUN_0891aa1c(int);
extern void FUN_08acf984(int);
extern void FUN_08ad03c0(int, int);
extern void FUN_08ad06e0(int, void *);
void FUN_08914770(struct Owner *self)
{
    self->active = FUN_08baa89c() == 1;
    FUN_08baa8d4(0x300);
    FUN_08baa8d4(0x301);
    FUN_08baa8d4(0x302);
    FUN_08ac6c24(0xc20, 0x1b, 0, 4);
    FUN_08ac7ba0(FUN_08913e60, FUN_08913e7c);
    FUN_089188a0(self->list);
    int setup = FUN_0891a58c();
    setup = FUN_0891aa1c(setup);
    FUN_08acf984(2);
    FUN_08ad03c0(0x20, 0x400);
    FUN_08ad06e0(0, self->value);
}
