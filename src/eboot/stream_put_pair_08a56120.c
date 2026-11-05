/* LBPMATCH module=EBOOT.BIN start=0x08a56120 end=0x08a56258 symbol=FUN_08a56120 lang=c99 cc=gcc404xs-O2 */
/* Hand-written reconstruction from the disassembly (lane lbp11a, Claude Sonnet 5.5; exact under psp-gcc-4.0.4w, the 4.0.4x kit adopted 2026-11-04); exact match verified; names and types await human review. */
typedef struct Stream {
    char pad0[4];
    char *cur;
    char pad1[0x0c];
    int count;
    char pad2[0x18];
    int owner;
} Stream;
typedef struct Wrap { Stream *s; } Wrap;
typedef struct Src { float a; float b; int c; } Src;
extern unsigned char FUN_08954cc4(int p);
static inline void put_float(Wrap *w, float v)
{
    Stream *s = w->s;
    if (!FUN_08954cc4(s->owner)) {
        __builtin_memcpy(s->cur, &v, 4);
    }
    s->cur += 4;
    s->count += 4;
}
static inline void put_int(Wrap *w, int v)
{
    Stream *s = w->s;
    if (!FUN_08954cc4(s->owner)) {
        __builtin_memcpy(s->cur, &v, 4);
    }
    s->cur += 4;
    s->count += 4;
}
void FUN_08a56120(Wrap *w, Src *src)
{
    float y = src->b;
    put_float(w, src->a);
    if (y < 0.0f) y = -y;
    put_float(w, y);
    put_int(w, src->c);
}
