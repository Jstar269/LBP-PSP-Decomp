/* LBPMATCH module=EBOOT.BIN start=0x088bbf6c end=0x088bc040 symbol=FUN_088bbf6c lang=c++ */
/* Hand-written reconstruction from the disassembly (lane lbp18a, Codex GPT-6); exact match verified; names and types await human review. */
struct Mutex { int a; };
extern Mutex DAT_08bef8f4;
extern "C" void FUN_08ad3088(void *);
extern "C" void FUN_08ad2810(void *);
extern "C" void FUN_088bb960(void *, int);
extern "C" void FUN_088bb9d0(void *, void *);
struct Big {
    void **vt;
    int count;
    char pad[576];
    int f584;
    char pad2[132];
    void (*f720)(void *, int);
    void (*f724)(void *, void *);
    void *f728;
    void *f732;
};
struct Handle {
    Big *p;
    ~Handle()
    {
        Big *r = p;
        if (r) {
            FUN_08ad3088(&DAT_08bef8f4);
            int c = r->count - 1;
            r->count = c;
            FUN_08ad2810(&DAT_08bef8f4);
            if (c == 0) ((void (*)(Big *))r->vt[2])(r);
        }
    }
};
struct Owner { char pad[88]; unsigned char flag; };
extern "C" void FUN_0883a3c8(Handle *);
extern "C" void FUN_08921bec(Handle *);
extern "C" void FUN_08922ce0(Handle *, int, int, int);
extern "C" void FUN_08922a00(Handle *, int);

extern "C" void FUN_088bbf6c(Owner *self)
{
    self->flag = 0;
    Handle h;
    FUN_0883a3c8(&h);
    FUN_08921bec(&h);
    FUN_08922ce0(&h, 572, 0, 0);
    Big *b = h.p;
    b->f584 = 400;
    b->f720 = FUN_088bb960;
    b->f724 = FUN_088bb9d0;
    b->f728 = self;
    b->f732 = self;
    FUN_08922a00(&h, 0);
}
