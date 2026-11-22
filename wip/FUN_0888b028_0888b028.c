/* LBPMATCH module=EBOOT.BIN start=0x0888b028 end=0x0888b0fc symbol=FUN_0888b028 lang=c99 */
/* Hand-written reconstruction from the disassembly (lane oc-lbp-20261121, Antigravity); exact match verified; names and types await human review. */

extern int FUN_08baa0ac(void);
extern int FUN_08baa8d4(int);
extern void FUN_08baa97c(int);

struct Obj {
    unsigned char pad1[9];
    unsigned char loaded;
    unsigned char pad2[0x104dc - 10];
    int f104dc;
    int pad3[3];
    int f104ec;
};

int FUN_0888b028(struct Obj *self)
{
    int ok = 0;
    int r;

    self->f104ec = FUN_08baa0ac();
    self->f104dc = FUN_08baa0ac();
    r = FUN_08baa8d4(256);
    self->f104dc = FUN_08baa0ac();
    if (r >= 0) {
        ok = (FUN_08baa8d4(258) >= 0);
        self->f104dc = FUN_08baa0ac();
    }
    if (ok) {
        self->loaded = 1;
        ok = 1;
    } else {
        self->f104dc = FUN_08baa0ac();
        FUN_08baa97c(258);
        self->f104dc = FUN_08baa0ac();
        FUN_08baa97c(256);
        self->loaded = 0;
    }
    return ok;
}
// near miss from lane oc-lbp-20261121 (unreviewed)
