/* LBPMATCH module=EBOOT.BIN start=0x0887e580 end=0x0887e610 symbol=FUN_0887e580 lang=c99 */
/* Hand-written reconstruction from the disassembly (lane api-lbp-a-20261121, Claude Sonnet 5.5); near miss, NOT exact; names and types await human review. */
extern void FUN_0888b8c4(void *);
extern void FUN_0888b8cc(void *);
extern void FUN_08baa4b4(int);
extern void FUN_0887c784(int, void *);
extern int DAT_08bedba4;
struct O {
    int pad[2];
    unsigned char done;
    char pad2[3];
    void *cb;
    int pad3[3];
    int fd;
    char lock[4];
};
void FUN_0887e580(struct O *o)
{
    int neg;
    FUN_0888b8c4(o->lock);
    neg = o->fd < 0;
    if (!neg)
        FUN_08baa4b4(o->fd);
    o->done = 1;
    FUN_0888b8cc(o->lock);
    if (o->cb && !neg)
        FUN_0887c784(DAT_08bedba4, o->cb);
}
// NEAR 6 words: retail 'slti s2,v0,0' for the fd<0 flag where ours emits 'srl s2,v0,0x1f' (int/char/short/_Bool/bool all give srl); the tail scheduling differs as a consequence.
// Tried: int/char/bool types, fd>=0 flag (28 words off), local fd.
// near miss from lane api-lbp-a-20261121 (unreviewed)
