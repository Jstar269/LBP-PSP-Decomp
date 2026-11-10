/* LBPMATCH module=EBOOT.BIN start=0x088d3ef0 end=0x088d4084 symbol=_ZN16RefList_088d3ef0aSERKS_ lang=c++ */
/* Hand-written reconstruction from the disassembly (lane lbp21h, Claude Opus 5.5): a std::list<Ref>::operator= instance (libstdc++ 4.0 list.tcc shape, written with plain iterator classes); exact match verified; names and types await human review. */
inline void *operator new(unsigned, void *p) throw() { return p; }
extern "C" void *DAT_08be84a0;
extern "C" void FUN_08adbc24(void *heap, void *p, unsigned size);
extern "C" void *FUN_08adbb84(void *heap, unsigned size);
struct Counted {
    virtual ~Counted();
    int refs;
};
struct Ref {
    Counted *p;
    Ref(const Ref &o) : p(o.p) { if (p) p->refs++; }
    ~Ref() { if (p && --p->refs == 0) delete p; }
    Ref &operator=(const Ref &o) {
        Counted *q = o.p;
        if (q) q->refs++;
        Counted *old = p;
        p = q;
        if (old && --old->refs == 0) delete old;
        return *this;
    }
};
struct ListNodeBase { ListNodeBase *next; ListNodeBase *prev; };
struct ListNode : ListNodeBase { Ref value; };
extern "C" void FUN_08b88fac(ListNodeBase *node, ListNodeBase *pos);
extern "C" void FUN_08b88fc8(ListNodeBase *node);
struct Iter {
    ListNodeBase *node;
    Iter() {}
    explicit Iter(ListNodeBase *n) : node(n) {}
    Ref &operator*() const { return ((ListNode *)node)->value; }
    Iter &operator++() { node = node->next; return *this; }
    Iter operator++(int) { Iter tmp = *this; node = node->next; return tmp; }
    bool operator==(const Iter &o) const { return node == o.node; }
    bool operator!=(const Iter &o) const { return node != o.node; }
};
struct ConstIter {
    const ListNodeBase *node;
    ConstIter() {}
    explicit ConstIter(const ListNodeBase *n) : node(n) {}
    const Ref &operator*() const { return ((const ListNode *)node)->value; }
    ConstIter &operator++() { node = node->next; return *this; }
    ConstIter operator++(int) { ConstIter tmp = *this; node = node->next; return tmp; }
    bool operator==(const ConstIter &o) const { return node == o.node; }
    bool operator!=(const ConstIter &o) const { return node != o.node; }
};
struct RefList_088d3ef0 {
    ListNodeBase head;
    Iter begin() { return Iter(head.next); }
    Iter end() { return Iter(&head); }
    ConstIter begin() const { return ConstIter(head.next); }
    ConstIter end() const { return ConstIter(&head); }
    ListNode *create_node(const Ref &v) {
        ListNode *n = (ListNode *)FUN_08adbb84(DAT_08be84a0, 12);
        new (&n->value) Ref(v);
        return n;
    }
    void insert_node(Iter pos, const Ref &v) {
        ListNode *n = create_node(v);
        FUN_08b88fac(n, pos.node);
    }
    Iter erase(Iter pos) {
        Iter ret = Iter(pos.node->next);
        FUN_08b88fc8(pos.node);
        ListNode *n = (ListNode *)pos.node;
        n->value.~Ref();
        FUN_08adbc24(DAT_08be84a0, n, 12);
        return ret;
    }
    Iter erase(Iter first, Iter last) {
        while (first != last)
            first = erase(first);
        return last;
    }
    void insert(Iter pos, ConstIter first, ConstIter last) {
        for (; first != last; ++first)
            insert_node(pos, *first);
    }
    RefList_088d3ef0 &operator=(const RefList_088d3ef0 &x);
};
RefList_088d3ef0 &RefList_088d3ef0::operator=(const RefList_088d3ef0 &x)
{
    if (this != &x) {
        Iter first1 = begin();
        Iter last1 = end();
        ConstIter first2 = x.begin();
        ConstIter last2 = x.end();
        for (; first1 != last1 && first2 != last2; ++first1, ++first2)
            *first1 = *first2;
        if (first2 == last2)
            erase(first1, last1);
        else
            insert(last1, first2, last2);
    }
    return *this;
}
