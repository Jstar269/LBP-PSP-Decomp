/* LBPMATCH module=EBOOT.BIN start=0x088b61bc end=0x088b6260 symbol=FUN_088b61bc lang=c99 */
/* Hand-written reconstruction from the disassembly (lane oc-lbp-20261121, Antigravity); exact match verified; names and types await human review. */

struct Tex {
    int f0;
    unsigned int w;
    unsigned int h;
};

struct Sub {
    unsigned char pad[0xa8];
    struct Tex *tex;
};

struct Obj {
    unsigned char pad[0xc4];
    struct Sub *sub;
};

struct Gpu {
    int f0;
    unsigned int *cmd;
};

extern struct Gpu *DAT_08bdc188;
extern void FUN_08a24424(struct Tex *, struct Gpu *);
extern void FUN_08a23f64(int, int, int, int, int, int, unsigned int, unsigned int, int);

void FUN_088b61bc(struct Obj *self)
{
    if (self->sub != 0) {
        struct Tex *t = self->sub->tex;
        struct Gpu *g;
        unsigned int *cmd;
        FUN_08a24424(t, DAT_08bdc188);
        g = DAT_08bdc188;
        cmd = g->cmd;
        cmd[0] = 0x21000001;
        cmd[1] = 0xdf000036;
        cmd[2] = 0xe0000000;
        cmd[3] = 0xe1000000;
        g->cmd = cmd + 4;
        FUN_08a23f64(0, 0, 480, 272, 0, 0, t->w, t->h, 0xffff);
    }
}
