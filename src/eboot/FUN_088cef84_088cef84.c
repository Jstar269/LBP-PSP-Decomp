/* LBPMATCH module=EBOOT.BIN start=0x088cef84 end=0x088cf014 symbol=FUN_088cef84 lang=c99 */
/* Hand-written reconstruction from the disassembly (lane oc-lbp-20261121, Antigravity); exact match verified; names and types await human review. */

struct State {
    unsigned char pad1[0xd3];
    unsigned char flags;
    unsigned char pad2[0xdc - 0xd4];
    void *f0dc;
};

extern void *DAT_08beed14;
extern struct State *DAT_08beee0c;
extern int FUN_088e6aa4(struct State *, void *);
extern int FUN_088e68a0(struct State *, int, int);
extern int FUN_088dd588(void *, void *, int);

int FUN_088cef84(void)
{
    int r = FUN_088e6aa4(DAT_08beee0c, (void *)FUN_088cef84);
    if (r != 0) {
        return r;
    }
    if ((DAT_08beee0c->flags & 1) != 0) {
        return FUN_088e68a0(DAT_08beee0c, 1, 0x3d);
    }
    FUN_088e68a0(DAT_08beee0c, 1, 0x3b);
    return FUN_088dd588(DAT_08beed14, DAT_08beee0c->f0dc, 0);
}
