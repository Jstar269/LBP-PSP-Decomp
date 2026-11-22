/* LBPMATCH module=EBOOT.BIN start=0x0894dab8 end=0x0894db7c symbol=FUN_0894dab8 lang=c99 */
/* Hand-written reconstruction from the disassembly (lane api-lbp-a-20261121, Claude Sonnet 5.5); exact match verified; names and types await human review. */
extern void FUN_08a9df6c(void *);
extern unsigned FUN_08ba0e8c(const char *);
extern void *FUN_08a9e4a4(unsigned, int, int);
extern void FUN_08ba0868(void *, unsigned, const char *);
struct S {
    char pad[28];
    void *name;
    char pad1[5];
    unsigned char len;
};
void FUN_0894dab8(struct S *s, const char *str, unsigned char len)
{
    if (s->name) {
        FUN_08a9df6c(s->name);
        s->len = 0;
        s->name = 0;
    }
    if (str) {
        if (len == 0)
            len = FUN_08ba0e8c(str);
        s->len = len;
        if (len) {
            s->name = FUN_08a9e4a4(len + 1, 0, 0);
            FUN_08ba0868(s->name, len + 1, str);
        }
    }
}
