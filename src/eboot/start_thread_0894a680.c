/* LBPMATCH module=EBOOT.BIN start=0x0894a680 end=0x0894a780 symbol=FUN_0894a680 lang=c99 */
/* Hand-written reconstruction from the disassembly (lane api-lbp-a-20261121, Claude Sonnet 5.5); exact match verified; names and types await human review. */
extern void FUN_0894a49c(void *);
extern void FUN_08ad1b5c(void *, int);
extern void FUN_08ad3088(void *);
extern void FUN_08ad2810(void *);
extern void FUN_08ad07e0(void *);
extern void FUN_08ad0990(void *, int);
extern void FUN_08ad08ac(void *, int);
extern void FUN_08ad0900(void *, char *);
extern void FUN_08ad09f8(void *, void *);
extern void FUN_08ad08e4(void *, void *);
extern void FUN_08ad09bc(void *, int);
extern void FUN_08ad13f8(void *, void *, void *, void *);
extern void FUN_08ad089c(void *);
extern int FUN_0894a5d8(void *);
struct S {
    char pad0[4];
    char thr[4];
    char ev[4];
    char pad1[8];
    char opt[0x198];
    char lock[148];
    int fn;
    int arg;
    unsigned char count;
};
void FUN_0894a680(struct S *s, int a1, unsigned char a2, int fn, char *name, int arg)
{
    char *lock = s->lock;
    int attr[12];
    s->fn = fn;
    s->arg = arg;
    FUN_0894a49c(lock);
    FUN_08ad1b5c(s->ev, 0);
    FUN_08ad3088(lock);
    s->count = s->count + 1;
    FUN_08ad2810(lock);
    attr[0] = a1;
    FUN_08ad07e0(attr + 1);
    FUN_08ad0990(attr + 1, 1);
    FUN_08ad08ac(attr + 1, a2);
    FUN_08ad0900(attr + 1, name);
    FUN_08ad09f8(attr + 1, attr);
    FUN_08ad08e4(attr + 1, s->opt);
    FUN_08ad09bc(attr + 1, 0x8000);
    FUN_08ad13f8(s->thr, attr + 1, FUN_0894a5d8, s);
    FUN_08ad089c(attr + 1);
}
