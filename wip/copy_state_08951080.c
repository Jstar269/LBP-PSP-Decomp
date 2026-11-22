/* LBPMATCH module=EBOOT.BIN start=0x08951080 end=0x0895116c symbol=FUN_08951080 lang=c99 */
/* Hand-written reconstruction from the disassembly (lane api-lbp-a-20261121, Claude Sonnet 5.5); near miss, NOT exact; names and types await human review. */
extern void FUN_08a1be54(void *);
extern void FUN_08a25718(int);
extern void FUN_08a257d0(float, float);
extern void FUN_08a25654(int);
struct Big { int w[79]; };
struct T { int pad; float fx, fy; int flag; };
struct P {
    int id;
    int state;
    int pad[72];
    int f296;
    struct T t;
};
struct G { char pad[1196]; struct Big copy; char pad2[1512 - 1196 - 316]; unsigned char ready; };
extern struct G *DAT_08be8510;
struct O { int pad; int adj; };
void FUN_08951080(struct O *o, int base)
{
    struct P *p = (struct P *)(base + o->adj);
    struct G *g;
    int st;
    if (p->id == -1)
        FUN_08a1be54((char *)p + 12);
    st = p->state;
    if (st == -1) {
        FUN_08a25718(p->f296);
        if (p->state == -1) {
            struct T *t = &p->t;
            FUN_08a257d0(t->fx, t->fy);
            FUN_08a25654(t->flag != 0);
        }
    }
    g = DAT_08be8510;
    if (g) {
        g->copy = *(struct Big *)p;
        g->ready = 1;
    }
}
// NEAR 9 words (59 of 59): everything but the inline 316-byte block-copy loop matches: retail's loop body is 'sw v0,0(a2); addiu s0,s0,16; addiu a2,a2,16; sw v1,-12(a2); sw a0,-8(a2); sw a1,-4(a2)' (destination pointer bumped in the middle, negative offsets), ours bumps after the stores with positive offsets, and the final 'sb v0,1512(a3)' sits between two of the tail stores in retail. Same block-move expander difference as the lane lbp10b notes (0x08b2d290 etc.).
// near miss from lane api-lbp-a-20261121 (unreviewed)
