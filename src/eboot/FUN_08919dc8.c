/* LBPMATCH module=EBOOT.BIN start=0x08919dc8 end=0x08919e60 symbol=FUN_08919dc8 lang=c99 */
/* Hand-written reconstruction from the disassembly (lane lbp18b, GPT-6); exact match verified; names and types await human review. */
typedef unsigned int uint;
struct Buffer { int unk0; unsigned char flag1; unsigned char flag2; char pad[2]; int value; };
extern void *DAT_08be84a0;
extern void *FUN_08a9e838(unsigned int);
extern void FUN_0891be44(struct Buffer *, int);
extern void *FUN_08adbb84(void *, unsigned int);
extern void FUN_08b88fac(void *, void *);
void FUN_08919dc8(void *self, int value)
{
    if (value != 0) {
        struct Buffer *buffer = FUN_08a9e838(0xc);
        FUN_0891be44(buffer, value);
        void *list = (char *)self + 8;
        void *node = FUN_08adbb84(DAT_08be84a0, 0xc);
        char *payload = (char *)node + 8;
        if (payload != 0)
            *(void **)payload = buffer;
        FUN_08b88fac(node, list);
    }
}
