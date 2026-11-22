/* LBPMATCH module=EBOOT.BIN start=0x088622a4 end=0x08862360 symbol=FUN_088622a4 lang=c99 */
/* Hand-written reconstruction from the disassembly (lane oc-lbp-20261121, Antigravity); exact match verified; names and types await human review. */

extern int DAT_08bafb68;
extern char DAT_08bafbf4[];

struct Global {
    unsigned char pad[0x29c];
    int lang;
};
extern struct Global *DAT_08be85fc;

extern void FUN_0885c984(void *, int, int, int, int, int);
extern char *FUN_08887630(void);
extern int FUN_08887898(void);
extern int FUN_088876b8(void);
extern int FUN_0888287c(int, int);
extern int FUN_088827d8(int);
extern int FUN_08ba0868(char *, int, const char *, ...);
extern void FUN_0885d244(void *, char *);

struct Obj {
    void *vt;
    unsigned char pad1[0xca];
    unsigned char flag;
    unsigned char pad2[0x1d0 - 0xcf];
    void *ref;
};

void FUN_088622a4(struct Obj *self)
{
    char buf[256];
    char *s;
    int a, b, u4, u5;

    FUN_0885c984(self, 0, 0, 0, 0, 1);
    self->vt = &DAT_08bafb68;
    self->ref = 0;
    self->flag = 0;
    s = FUN_08887630();
    a = FUN_08887898();
    b = FUN_088876b8();
    u4 = FUN_0888287c(a, b);
    u5 = FUN_088827d8(DAT_08be85fc->lang);
    FUN_08ba0868(buf, 0x100, DAT_08bafbf4, s, u4, u5);
    FUN_0885d244(self, buf);
}
