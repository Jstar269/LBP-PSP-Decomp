/* LBPMATCH module=EBOOT.BIN start=0x088a29a4 end=0x088a2b34 symbol=FUN_088a29a4 lang=c99 */
/* Hand-written reconstruction from the disassembly (lane lbp21o, Claude Opus 5.5); exact match verified; names and types await human review. */
struct Prop;
struct PropVtbl { void (*slot[4])(struct Prop *); void (*changed)(struct Prop *); };
struct Prop { struct PropVtbl *vtbl; int pad04[4]; int *values; };
struct Panel {
    char pad00[124];
    struct Prop mode;
    char pad94[152 - 124 - 24];
    struct Prop height;
    char padb0[180 - 152 - 24];
    struct Prop width;
};
struct Info { char raw[160]; };
extern int DAT_08bdb200;
extern int DAT_08bdb204;
extern int DAT_08bee3b0;
extern void FUN_08baa57c(struct Info *info);
extern int FUN_0889d194(void);
extern int FUN_0889d08c(int ctx);
extern int FUN_089ecaa8(void);
extern int FUN_089ea98c(int screen);
extern int FUN_089ea984(int screen);
static inline void prop_set(struct Prop *p, int v)
{
    int idx = FUN_0889d08c(FUN_0889d194());
    int *slot = &p->values[idx];
    if (v != *slot) {
        *slot = v;
        p->vtbl->changed(p);
    }
}
void FUN_088a29a4(struct Panel *o)
{
    struct Info info;
    FUN_08baa57c(&info);
    prop_set(&o->mode, 1);
    DAT_08bdb200 = -1;
    DAT_08bee3b0 = 0;
    prop_set(&o->width, FUN_089ea98c(FUN_089ecaa8()));
    prop_set(&o->height, FUN_089ea984(FUN_089ecaa8()));
    if (DAT_08bdb204 == 5)
        prop_set(&o->width, FUN_089ea98c(FUN_089ecaa8()) + 100);
}
