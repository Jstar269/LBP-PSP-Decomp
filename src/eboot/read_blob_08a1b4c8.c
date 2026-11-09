/* LBPMATCH module=EBOOT.BIN start=0x08a1b4c8 end=0x08a1b57c symbol=FUN_08a1b4c8 lang=c99 cc=gcc404xs-O2 */
/* Hand-written reconstruction from the disassembly (lane lbp10c, Claude Sonnet 5.5); exact match verified under cc=gcc404xs-O2 after a compiler-profile sweep (lane lbp20s, Claude Opus 5.5); names and types await human review. */
typedef struct Blob { void *buf; int pad4; unsigned int len; } Blob;
typedef struct Reader { int pad0; char *base; char *cur; int pad0c; int off; } Reader;
extern void FUN_08a9df6c(void *p);
extern void *FUN_08a9ecb8(unsigned int n, int a, int b, int c);
extern void *FUN_08b9e7a8(void *d, const void *s, unsigned int n);
void FUN_08a1b4c8(Blob *b, Reader *r)
{
    if (b->buf) FUN_08a9df6c(b->buf);
    __builtin_memcpy(&b->len, r->cur, 4);
    int off = r->off + 4;
    r->off = off;
    r->cur = r->base + off;
    b->buf = FUN_08a9ecb8(b->len, 64, 0, 0);
    unsigned int n = b->len;
    FUN_08b9e7a8(b->buf, r->cur, n);
    int off2 = n + r->off;
    r->off = off2;
    r->cur = r->base + off2;
}
