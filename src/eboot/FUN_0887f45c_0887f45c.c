/* LBPMATCH module=EBOOT.BIN start=0x0887f45c end=0x0887f4fc symbol=FUN_0887f45c lang=c99 */
/* Hand-written reconstruction from the disassembly (lane oc-lbp-20261121, Antigravity); exact match verified; names and types await human review. */

struct Obj {
    void (**vt)(struct Obj *, void (*)(void), void *);
};

extern struct Obj *DAT_08bedd04;
extern struct Obj *DAT_08bedc38;
extern struct Obj *DAT_08bedc30;
extern struct Obj *DAT_08bedc34;

extern void FUN_0887f1b4(void);
extern void FUN_0887f1bc(void);
extern void FUN_0887f1c4(void);
extern void FUN_0887f1cc(void);

void FUN_0887f45c(void *param_1)
{
    DAT_08bedd04->vt[2](DAT_08bedd04, FUN_0887f1b4, param_1);
    DAT_08bedc38->vt[2](DAT_08bedc38, FUN_0887f1bc, param_1);
    DAT_08bedc30->vt[2](DAT_08bedc30, FUN_0887f1c4, param_1);
    DAT_08bedc34->vt[2](DAT_08bedc34, FUN_0887f1cc, param_1);
}
