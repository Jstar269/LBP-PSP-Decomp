/* LBPMATCH module=EBOOT.BIN start=0x08872ea0 end=0x08873034 symbol=FUN_08872ea0 lang=c99 */
/* Hand-written reconstruction from the disassembly (lane lbp19p, Claude Opus 5.5); exact match verified; names and types await human review. */
struct Ref;
struct RefVt { void *slot0; void (*destroy)(struct Ref *); };
struct Ref { struct RefVt *vt; int count; };
struct Inner { struct Inner *next; struct Inner *prev; int pad; struct Ref *ref; };
struct InnerList { struct Inner *next; struct Inner *prev; };
struct Elem { int key; struct Ref *ref; struct InnerList inner; int pad[7]; };
struct Outer { struct Outer *next; struct Outer *prev; struct Elem elem; };
struct OuterList { struct Outer *next; struct Outer *prev; };
struct Pool { char pad[32]; char lock[1]; };
struct Self { void *vt; int pad[12]; struct OuterList list; };
extern char DAT_08bef8f4[];
extern void *DAT_08bb0518[];
extern struct Pool *DAT_08be84ac;
extern void FUN_08ad3088(void *);
extern void FUN_08ad2810(void *);
extern void FUN_08adbc24(struct Pool *, void *, int);
extern void FUN_0886cd04(struct Self *);
extern void FUN_08a9e0cc(void *);
static inline void release(struct Ref *r) {
    int n;
    FUN_08ad3088(DAT_08bef8f4);
    n = --r->count;
    FUN_08ad2810(DAT_08bef8f4);
    if (n == 0) r->vt->destroy(r);
}
static inline void pool_free(void *p, int size) {
    struct Pool *pool = DAT_08be84ac;
    FUN_08ad3088(pool->lock);
    FUN_08adbc24(pool, p, size);
    FUN_08ad2810(pool->lock);
}
void FUN_08872ea0(struct Self *self) {
    struct Outer *oend = (struct Outer *)&self->list;
    struct Outer *o;
    self->vt = DAT_08bb0518;
    o = self->list.next;
    while (o != oend) {
        struct Outer *onext = o->next;
        struct Inner *n = o->elem.inner.next;
        struct Elem *e = &o->elem;
        struct Inner *end = (struct Inner *)&e->inner;
        while (n != end) {
            struct Inner *next = n->next;
            struct Ref *r = n->ref;
            if (r != 0) release(r);
            pool_free(n, sizeof(struct Inner));
            n = next;
        }
        if (e->ref != 0) release(e->ref);
        pool_free(o, sizeof(struct Outer));
        o = onext;
    }
    FUN_0886cd04(self);
    FUN_08a9e0cc(self);
}
