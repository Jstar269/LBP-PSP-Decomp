/* LBPMATCH module=EBOOT.BIN start=0x08887c4c end=0x08887d30 symbol=FUN_08887c4c lang=c99 */
/* Hand-written reconstruction from the disassembly (lane oc-lbp-20261121, Antigravity); exact match verified; names and types await human review. */

extern void FUN_0888f6c8(void *);
extern void FUN_0888f6d0(void *);

struct Obj {
    unsigned int flags;
    int lock;
};

struct Msg {
    int type;
    int code;
};

void FUN_08887c4c(struct Obj *self, struct Msg *msg)
{
    void *lock = &self->lock;
    int type;

    FUN_0888f6c8(lock);
    type = msg->type;
    if (type == 3) {
        if (msg->code == (int)0x80431068) {
            self->flags |= 8;
        }
        self->flags |= 0x20;
    } else if (type == 2) {
        int code = msg->code;
        if (code == 403) {
            self->flags |= 1;
        } else if (code == 418) {
            self->flags |= 2;
        } else if (code == 500) {
            self->flags |= 4;
        }
    } else if (type == 1 || type == 4) {
        self->flags |= 0x10;
    }
    FUN_0888f6d0(lock);
}
