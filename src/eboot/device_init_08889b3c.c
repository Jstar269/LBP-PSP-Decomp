/* LBPMATCH module=EBOOT.BIN start=0x08889b3c end=0x08889cd4 symbol=FUN_08889b3c lang=c99 */
/* Hand-written reconstruction from the disassembly (lane lbp21o, Claude Opus 5.5); matching idiom: goto to the open_device block written after the return (retail places it at the end of the function); exact match verified; names and types await human review. */
struct Obj {
    int f0;
    int f4;
    unsigned char ready;
    char pad[0x104dc - 9];
    int status;
    int start;
    int f104e4;
};
extern int FUN_08baa0ac(void);
extern int FUN_08baa8d4(int id);
extern int FUN_08887670(void);
extern int FUN_08887664(void);
extern int FUN_08baa49c(int a);
extern int FUN_08baa87c(int a);
extern int FUN_08baa4cc(int a, int b, int c, int d);
extern int FUN_08baa46c(int a, int b);
extern void FUN_08baa4a4(int a);
extern int FUN_08baa4dc(void);
extern void FUN_08889ad0(struct Obj *o);
static inline int track(struct Obj *o, int r)
{
    o->status = FUN_08baa0ac();
    return r;
}
int FUN_08889b3c(struct Obj *o)
{
    int result;
    o->start = FUN_08baa0ac();
    o->status = FUN_08baa0ac();
    if (track(o, FUN_08baa8d4(259)) < 0)
        goto fail;
    if (track(o, FUN_08baa8d4(260)) < 0)
        goto fail;
    if (track(o, FUN_08baa8d4(261)) < 0)
        goto fail;
    if (track(o, FUN_08baa8d4(262)) >= 0)
        goto open_device;
    goto fail;
start_device:
    if (track(o, FUN_08baa49c(FUN_08887670())) < 0)
        goto fail;
    if (track(o, FUN_08baa4cc(0, 0, 0, 0)) < 0)
        goto fail;
    if (track(o, FUN_08baa46c(0, 0)) < 0)
        goto fail;
    FUN_08baa4a4(1);
    if (track(o, FUN_08baa4dc()) < 0)
        goto fail;
    o->ready = 1;
    result = 1;
    goto done;
fail:
    result = 0;
done:
    if (!o->ready)
        FUN_08889ad0(o);
    return result;
open_device:
    o->f104e4 = FUN_08baa0ac();
    o->status = FUN_08baa0ac();
    if (track(o, FUN_08baa87c(FUN_08887664())) >= 0)
        goto start_device;
    goto fail;
}
