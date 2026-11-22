/* LBPMATCH module=EBOOT.BIN start=0x088c9310 end=0x088c9410 symbol=FUN_088c9310 lang=c++ */
/* Hand-written reconstruction from the disassembly (lane oc-lbp-20261121, Antigravity); exact match verified; names and types await human review. */

extern "C" int DAT_08bef8f4;
extern "C" void *DAT_08be84a0;
extern "C" void FUN_08b88fc8(void *);
extern "C" void FUN_08ad3088(void *);
extern "C" void FUN_08ad2810(void *);
extern "C" void FUN_08adbc24(void *, void *, int);
extern "C" void FUN_089475dc(int);
extern "C" void FUN_088c8e1c(void *);

struct Global {
    unsigned char pad1[0x614];
    int lock;
    unsigned char pad2[0x6b0 - 0x618];
    unsigned char flags[16];
    inline void set_flag(int idx)
    {
        FUN_08ad3088(&lock);
        flags[idx] |= 3;
        FUN_08ad2810(&lock);
    }
};

extern "C" Global *DAT_08bedb1c;

struct Task {
    void (**vt)(Task *);
    int rc;
};

struct TaskPtr {
    Task *p;
    inline ~TaskPtr()
    {
        Task *r = p;
        if (r != 0) {
            FUN_08ad3088(&DAT_08bef8f4);
            int c = --r->rc;
            FUN_08ad2810(&DAT_08bef8f4);
            if (c == 0) {
                r->vt[1](r);
            }
        }
    }
};

struct Node {
    Node *next;
    Node *prev;
    TaskPtr val;
};

struct Obj {
    unsigned char pad1[0x60];
    int state;
    unsigned char pad2[0x84 - 0x64];
    Node *list_next;
    unsigned char pad3[0xb4 - 0x88];
    int idx;
};

extern "C" void FUN_088c9310(Obj *self)
{
    Node *n = self->list_next;
    FUN_08b88fc8(n);
    n->val.~TaskPtr();
    FUN_08adbc24(DAT_08be84a0, n, 12);
    DAT_08bedb1c->set_flag(self->idx);
    FUN_089475dc(self->idx);
    if (self->idx == 0) {
        self->state = 0x1f;
    } else if (self->idx == 1) {
        self->state = 0x2d;
    }
    FUN_088c8e1c(self);
}
