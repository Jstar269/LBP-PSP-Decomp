/* LBPMATCH module=EBOOT.BIN start=0x088ccf1c end=0x088ccfb0 symbol=FUN_088ccf1c lang=c99 */
/* Hand-written reconstruction from disassembly (lane lbp17d, Codex GPT-6.1): clears the context flag and releases the context when the dispatch state permits; exact match verified; names and types await human review. */
extern void FUN_089ecaa8(void);
extern int *FUN_089ea97c(void);
extern void FUN_0883bd5c(int *, int);
extern void FUN_088b2aa4(int *);
extern void FUN_08804f3c(int *);
void FUN_088ccf1c(void *self)
{
    if (*((unsigned char *)self + 0x20) != 0) {
        FUN_089ecaa8();
        int *state = FUN_089ea97c();
        if (state != 0) {
            unsigned int mask = 1u << (state[0xb8 / 4] & 0x1f);
            unsigned int available = ~(unsigned int)state[2] & (unsigned int)state[1];
            if ((available & mask) != 0) {
                *((unsigned char *)self + 0x20) = 0;
                int object = *(int *)((char *)self + 0x14);
                if (object != 0) {
                    int stack[4];
                    FUN_0883bd5c(stack, object);
                    FUN_088b2aa4(stack);
                    FUN_08804f3c(stack);
                }
            }
        }
    }
}
