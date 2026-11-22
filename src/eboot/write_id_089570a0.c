/* LBPMATCH module=EBOOT.BIN start=0x089570a0 end=0x08957144 symbol=FUN_089570a0 lang=c99 */
/* Hand-written reconstruction from the disassembly (lane api-lbp-a-20261121, Claude Sonnet 5.5); exact match verified; names and types await human review. */
struct Q;
struct Obj { char pad[8]; unsigned char id[20]; char pad1[0]; };
struct Src { struct Obj *o; };
extern void FUN_0894b568(struct Q *, void *, int, int);
extern void FUN_0894b3e4(struct Q *, int);
struct Obj2 { char pad[28]; int f28; };
struct Src2 { struct Obj2 *o; };
static inline void put8(struct Q *q, unsigned char b) { FUN_0894b568(q, &b, 1, 1); }
static inline void put32(struct Q *q, int v) { FUN_0894b568(q, &v, 4, 1); }
struct Q *FUN_089570a0(int unused, struct Q *q, struct Src *s)
{
    const unsigned char *p = s->o->id;
    int i = 0;
    do {
        put8(q, *p);
        i++;
        p++;
    } while (i != 20);
    put32(q, ((struct Src2 *)s)->o->f28);
    FUN_0894b3e4(q, 4);
    return q;
}
