/* LBPMATCH module=EBOOT.BIN start=0x0888b3a0 end=0x0888b458 symbol=net_stack_term_core lang=c99 */
/* Hand-written reconstruction from the disassembly (lane oc-lbp-20261121, Antigravity); matching idiom: volatile on self->f54; exact match verified; names and types await human review. */

extern void FUN_08889ad0(void *);
extern void FUN_08baa5bc(int);
extern void FUN_08baa5cc(void);
extern void FUN_08baa66c(void);
extern void FUN_08baa5e4(void);
extern void FUN_08baa514(void);
extern int FUN_08baa0ac(void);
extern void FUN_08baa97c(int);

struct Obj {
    unsigned char pad1[9];
    unsigned char b09;
    unsigned char b0a;
    unsigned char pad2[0x50 - 11];
    int f50;
    volatile int f54;
    unsigned char pad3[0x104dc - 0x58];
    int f104dc;
};

void net_stack_term_core(struct Obj *self)
{
    FUN_08889ad0(self);
    if (self->b0a != 0) {
        if (self->f50 >= 0) {
            FUN_08baa5bc(self->f50);
            self->f50 = -1;
        }
        self->f54 = 0;
        FUN_08baa5cc();
        FUN_08baa66c();
        FUN_08baa5e4();
        FUN_08baa514();
        self->b0a = 0;
    }
    if (self->b09 != 0) {
        self->f104dc = FUN_08baa0ac();
        FUN_08baa97c(258);
        self->f104dc = FUN_08baa0ac();
        FUN_08baa97c(256);
        self->b09 = 0;
    }
}
