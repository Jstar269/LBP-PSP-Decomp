/* LBPMATCH module=EBOOT.BIN start=0x088e5170 end=0x088e5248 symbol=FUN_088e5170 lang=c++ rodata=0x08bb4884 */
/* Hand-written reconstruction from the disassembly (lane oc-lbp-20261121, Antigravity); exact match verified; names and types await human review. */

struct Sub {
    int kind;
    int f4;
    int f8;
    int f0c;
};

struct Net {
    unsigned char pad1[0x1c];
    Sub sub;
    unsigned char pad2[0x78 - 0x2c];
    char buf1[0x1a];
    char buf2[0x80];
};

struct Query {
    int kind;
    int start;
    int count;
    int f0c;
    char *p10;
    char *p14;
    int pad[2];
};

extern "C" void *DAT_08beee0c;
extern "C" const int DAT_08bb440c;
extern "C" int FUN_088e3e74(Net *, int, int, int);
extern "C" void FUN_088f3748(void *, Query *);

extern "C" void FUN_088e5170(Net *param_1)
{
    if (FUN_088e3e74(param_1, 10, 1, DAT_08bb440c) != 0) {
        Query q;
        Sub *s = &param_1->sub;
        q.count = s->f0c;
        q.start = (s->f8 - 1) * q.count + 1;
        switch (param_1->sub.kind) {
        case 10:
            q.kind = 0;
            break;
        case 11:
            q.kind = 9;
            break;
        case 12:
            q.kind = 8;
            break;
        case 13:
            q.kind = 7;
            q.p14 = param_1->buf2;
            break;
        case 14:
            q.kind = 4;
            q.p10 = param_1->buf1;
            break;
        }
        FUN_088f3748(DAT_08beee0c, &q);
    }
}
