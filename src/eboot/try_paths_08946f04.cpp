/* LBPMATCH module=EBOOT.BIN start=0x08946f04 end=0x08946fd0 symbol=FUN_08946f04 lang=c++ */
/* Hand-written reconstruction from the disassembly (lane api-lbp-a-20261121, Claude Sonnet 5.5); exact match verified; names and types await human review. */
struct Pair { int a; int b; };
struct Str { const char *p; int len; };
struct Big;
extern "C" void FUN_08a9c014(Big *);
extern "C" void FUN_08946ae4(int, Big *);
extern "C" int FUN_08a9d580(void *, void *);
extern "C" int FUN_08a9b8a4(Str *, const char *);
extern "C" void FUN_08a9bff8(Big *);
extern "C" const char *DAT_08bdb7f4[2];
struct Big {
    int a; int b; int rest[6];
    Big() { FUN_08a9c014(this); }
    ~Big() { FUN_08a9bff8(this); }
};
static inline int try_str(void *self, const char *s, Str *c)
{
    int n = FUN_08a9b8a4(c, s);
    c->p = s;
    c->len = n;
    return FUN_08a9d580(self, c);
}
extern "C" int FUN_08946f04(void *self)
{
    Big a;
    Str c;
    Pair b;
    const char **tp = DAT_08bdb7f4;
    int i = 0;
    do {
        FUN_08946ae4(i, &a);
        b.a = a.a;
        i++;
        b.b = a.b;
        if (FUN_08a9d580(self, &b) || try_str(self, *tp++, &c))
            return 1;
    } while (i != 2);
    return 0;
}
