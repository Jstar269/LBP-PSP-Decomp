/* LBPMATCH module=EBOOT.BIN start=0x08956b38 end=0x08956c34 symbol=FUN_08956b38 lang=c99 */
/* Hand-written reconstruction from the disassembly (lane api-lbp-a-20261121, Claude Sonnet 5.5); exact match verified; names and types await human review. */
struct Q;
struct Data { int pad; int type; };
extern void *FUN_08b9eb88(void *, int, int);
extern struct Data *FUN_0894b31c(struct Q *);
extern void FUN_0894b568(struct Data *, void *, int, int);
extern void FUN_0894b3e4(struct Data *, int);
static inline void put8(struct Data *q, unsigned char b) { FUN_0894b568(q, &b, 1, 1); }
static inline void put32(struct Data *q, int v) { FUN_0894b568(q, &v, 4, 1); }
void FUN_08956b38(int unused, struct Q *qq, int type)
{
    unsigned char buf[20];
    const unsigned char *p = buf;
    struct Data *q;
    int kind;
    FUN_08b9eb88(buf, 0, 20);
    q = FUN_0894b31c(qq);
    q->type = type;
    do {
        put8(q, *p);
        p++;
    } while (p != buf + 20);
    switch (type) {
    case 0x4d4c564c: kind = 2; break;
    case 0x4d4a424f: kind = 4; break;
    case 0x4d4f4349: kind = 8; break;
    case 0x4d544143: kind = 16; break;
    default: kind = 0; break;
    }
    put32(q, kind);
    FUN_0894b3e4(q, 4);
}
