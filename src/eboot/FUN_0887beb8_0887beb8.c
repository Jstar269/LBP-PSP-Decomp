/* LBPMATCH module=EBOOT.BIN start=0x0887beb8 end=0x0887bf8c symbol=FUN_0887beb8 lang=c99 */
/* Hand-written reconstruction from the disassembly (lane oc-lbp-20261121, Antigravity); exact match verified; names and types await human review. */

extern void FUN_0888f6c8(void *);
extern float FUN_088884b0(void);
extern void FUN_08887b48(void *, void *, float);
extern int FUN_08888394(long long *);
extern void FUN_0888f6d0(void *);

struct Obj {
    unsigned char pad[456];
    long long diff;
    int lock;
    int pad2;
    int f1d8;
};

void FUN_0887beb8(struct Obj *self, void *param_2, long long param_3)
{
    void *lock = &self->lock;
    long long now;
    long long diff;

    FUN_0888f6c8(lock);
    FUN_08887b48(&self->f1d8, param_2, FUN_088884b0());
    now = 0;
    if (param_3 != 0 && FUN_08888394(&now) != 0) {
        diff = now - param_3;
    } else {
        diff = 0;
    }
    self->diff = diff;
    FUN_0888f6d0(lock);
}
