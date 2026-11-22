/* LBPMATCH module=EBOOT.BIN start=0x0894d5e4 end=0x0894d68c symbol=FUN_0894d5e4 lang=c++ */
/* Hand-written reconstruction from the disassembly (lane api-lbp-a-20261121, Claude Sonnet 5.5); exact match verified; names and types await human review. */
struct Sub { char pad[20]; int flag; };
struct Item { char pad[16]; Sub sub; };
struct NodeBase { int color; NodeBase *parent; NodeBase *left; NodeBase *right; };
struct Node : NodeBase { int pad[1]; Item *item; };
struct Map {
    int compare;
    NodeBase header;
    unsigned count;
};
struct Self {
    virtual void v0(); virtual void v1(); virtual void v2(); virtual void v3();
    virtual void v4(); virtual void v5(); virtual void v6(); virtual void v7();
    virtual void v8(); virtual void v9(); virtual void v10();
    virtual int make(int a, void *p);
    virtual void v12(); virtual void v13(); virtual void v14();
    virtual void use(int h, Sub *s);
};
extern "C" NodeBase *FUN_08b898a0(NodeBase *);
extern "C" void FUN_0894d5e4(Self *s, int a, Map *m)
{
    if (m->count != 0) {
        for (NodeBase *n = m->header.left; n != &m->header; n = FUN_08b898a0(n)) {
            Item *it = static_cast<Node *>(n)->item;
            int h = s->make(a, (char *)it + 12);
            Sub *sub = &static_cast<Node *>(n)->item->sub;
            if (sub->flag)
                s->use(h, sub);
        }
    }
}
