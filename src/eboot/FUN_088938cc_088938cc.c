/* LBPMATCH module=EBOOT.BIN start=0x088938cc end=0x088939bc symbol=FUN_088938cc lang=c99 */
/* Hand-written reconstruction from the disassembly (lane oc-lbp-20261121, Antigravity); exact match verified; names and types await human review. */

extern void FUN_08adb8a4(void *);
extern unsigned char *FUN_08adb810(unsigned int, int, int, int);

struct Pool {
    unsigned char *mem;
    unsigned char **free_list;
    int cap;
    int used;
    int f10;
    unsigned char flag;
};

void FUN_088938cc(struct Pool *self, int n, unsigned char flag)
{
    if (self->mem != 0) {
        FUN_08adb8a4(self->mem);
    }
    self->mem = 0;
    self->free_list = 0;
    self->cap = n;
    self->used = 0;
    if (n > 0) {
        unsigned char *mem = FUN_08adb810(((unsigned int)(n * 228 + 15) & ~15u) + (unsigned int)(n * 4), 16, 0, 0);
        int cap = self->cap;
        unsigned char **fl = (unsigned char **)(mem + ((unsigned int)(cap * 228 + 15) & ~15u));
        int i;
        self->mem = mem;
        self->free_list = fl;
        for (i = 0; i < cap; i++) {
            fl[i] = self->mem + i * 228;
        }
    }
    self->flag = flag;
}
