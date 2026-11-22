/* LBPMATCH module=EBOOT.BIN start=0x0894ddc8 end=0x0894de68 symbol=FUN_0894ddc8 lang=c++ */
/* Hand-written reconstruction from the disassembly (lane api-lbp-a-20261121, Claude Sonnet 5.5); exact match verified; names and types await human review. */
struct NodeBase { int color; NodeBase *parent; NodeBase *left; NodeBase *right; };
struct Node : NodeBase { int pad[1]; char *item; };
struct Map {
    int compare;
    NodeBase header;
    unsigned count;
};
struct S {
    char pad[40];
    Map m1;
    Map m2;
};
extern "C" unsigned FUN_08903a50(S *);
extern "C" NodeBase *FUN_08b898a0(NodeBase *);
extern "C" void FUN_0894dd3c(void *, unsigned *);
extern "C" void FUN_088b83c8(Map *);
extern "C" void FUN_0894ddc8(S *s)
{
    Map *m = &s->m1;
    if (m->count != 0) {
        unsigned id = FUN_08903a50(s);
        for (NodeBase *n = m->header.left; n != &m->header; n = FUN_08b898a0(n)) {
            unsigned loc = id;
            FUN_0894dd3c(static_cast<Node *>(n)->item + 64, &loc);
        }
        FUN_088b83c8(m);
    }
    Map *m2 = &s->m2;
    if (m2->count != 0)
        FUN_088b83c8(m2);
}
