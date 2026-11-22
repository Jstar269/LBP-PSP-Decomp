/* LBPMATCH module=EBOOT.BIN start=0x0894f514 end=0x0894f5bc symbol=FUN_0894f514 lang=c++ */
/* Hand-written reconstruction from the disassembly (lane api-lbp-a-20261121, Claude Sonnet 5.5); exact match verified; names and types await human review. */
struct Inner { char pad[16]; int val; };
struct Mid { char pad[12]; Inner *in; };
struct Slot { Mid *mid; };
struct NodeBase { int color; NodeBase *parent; NodeBase *left; NodeBase *right; };
struct Node : NodeBase { int pad[1]; Slot item; };
struct Map {
    int compare;
    NodeBase header;
    unsigned count;
};
struct S {
    char pad[48];
    Map m;
};
extern "C" NodeBase *FUN_08b898a0(NodeBase *);
extern "C" int FUN_0894f2b4(S *, Slot *, int, int, unsigned char);
extern "C" int FUN_0894f514(S *s, int a, int b, unsigned char d)
{
    int n = 0;
    Map *m = &s->m;
    for (NodeBase *p = m->header.left; p != &m->header; p = FUN_08b898a0(p)) {
        Node *nd = static_cast<Node *>(p);
        n += FUN_0894f2b4(s, &nd->item, a, b, d) != 0;
    }
    return n;
}
