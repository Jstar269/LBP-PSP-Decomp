/* LBPMATCH module=EBOOT.BIN start=0x08883ba0 end=0x08883d54 symbol=FUN_08883ba0 lang=c99 rodata=0x08bb0c54 */
/* Hand-written reconstruction from the disassembly (lane lbp21o, Claude Opus 5.5); matching idioms: goto to the out-of-line upload/set_state blocks placed after the tail (retail block layout), volatile status count (retail reloads it); weak declarations of FUN_088829e8/FUN_088829f0 for the safe-bool address tests; exact match verified; names and types await human review. */
struct Status { int f0; volatile int count; };
struct Poster { void *vt; int rc; int pad8; struct Status status; };
struct Session { int state; int busy; char pad[984]; char data[4]; };
extern int DAT_08bedfac;
extern struct Poster *DAT_08bedfb4;
extern void *DAT_08bedfb0;
extern struct Session *DAT_08bee030;
extern void *DAT_08bee02c;
extern void *DAT_08bee004;
extern char DAT_08bee04c[];
extern void FUN_088829e8(void) __attribute__((weak));
extern void FUN_088829f0(void) __attribute__((weak));
extern void FUN_0887be18(void *o);
extern void FUN_08883694(void);
extern int FUN_08849ff0(struct Poster *p);
extern void FUN_0887c06c(void *o);
extern int FUN_08882a80(void);
extern void FUN_08b9eb88(void *p, int c, unsigned n);
extern void FUN_0888a574(struct Session *s);
extern void FUN_08882628(void);
extern void FUN_08880d34(void *o);
extern int FUN_0887bea8(void *o);
extern void FUN_0887d014(void *o);
extern void FUN_0888b458(struct Session *s);
void FUN_08883ba0(void)
{
    int next;
    switch (DAT_08bedfac) {
    case 1:
        if (DAT_08bee030->state != 3)
            break;
        DAT_08bee004 = DAT_08bee030->data;
        FUN_0887be18(DAT_08bee02c);
        FUN_08883694();
        next = 2;
        goto set_state;
    case 2:
        if (DAT_08bedfb4 && FUN_088829e8 && FUN_08849ff0(DAT_08bedfb4)) {
            next = 3;
            goto set_state;
        }
        break;
    case 4:
        if (DAT_08bedfb4 != 0) {
            struct Status *st = &DAT_08bedfb4->status;
            if (st->count >= 4 || st->count == 0)
                goto upload;
            break;
        }
        goto upload;
    case 5:
        if (!FUN_08882a80())
            break;
        FUN_08b9eb88(DAT_08bee04c, 0, 3);
        FUN_0888a574(DAT_08bee030);
        next = 6;
        goto set_state;
    case 6:
        if (DAT_08bee030->busy == 0 && DAT_08bee030->state == 0)
            DAT_08bedfac = 0;
        break;
    }
done:
    FUN_08882628();
    if (DAT_08bedfb0 && FUN_088829f0)
        FUN_08880d34(DAT_08bedfb0);
    if (!FUN_0887bea8(DAT_08bee02c))
        FUN_0887d014(DAT_08bee02c);
    FUN_0888b458(DAT_08bee030);
    return;
upload:
    FUN_0887c06c(DAT_08bee02c);
    next = 5;
set_state:
    DAT_08bedfac = next;
    goto done;
}
