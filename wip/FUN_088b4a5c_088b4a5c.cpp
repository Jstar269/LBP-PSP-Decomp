/* LBPMATCH module=EBOOT.BIN start=0x088b4a5c end=0x088b4b10 symbol=FUN_088b4a5c lang=c++ */
/* Hand-written reconstruction from the disassembly (lane oc-lbp-20261121, Antigravity); exact match verified; names and types await human review. */

extern "C" int DAT_08bef8f4;
extern "C" void FUN_08ad3088(void *);
extern "C" void FUN_08ad2810(void *);
extern "C" void FUN_0883bd5c(void *, void *);

struct Lock {
    int *m;
    inline Lock(int *p) : m(p) { FUN_08ad3088(m); }
    inline ~Lock() { FUN_08ad2810(m); }
};

struct SharedPtr {
    void *p;
    int *rc;
    inline SharedPtr(void *x)
    {
        FUN_0883bd5c(this, x);
    }
    inline SharedPtr(const SharedPtr &o)
    {
        int *r = o.rc;
        rc = r;
        p = o.p;
        if (r != 0) {
            FUN_08ad3088(&DAT_08bef8f4);
            ++*r;
            FUN_08ad2810(&DAT_08bef8f4);
        }
    }
};

struct NodeBase {
    NodeBase *next;
    NodeBase *prev;
};

struct Node : NodeBase {
    SharedPtr val;
};

struct List {
    NodeBase head;
    inline bool empty() const { return head.next == &head; }
    inline SharedPtr &back() const { return ((Node *)head.prev)->val; }
};

struct Obj {
    int pad[3];
    List list;
    int lock;
};

extern "C" SharedPtr FUN_088b4a5c(Obj *self)
{
    Lock g(&self->lock);
    if (self->list.empty()) {
        return SharedPtr(0);
    }
    return self->list.back();
}

// Remaining diffs (45/45 words; 4 differ at +0x5c): GCC loads/stores SharedPtr::p before rc whereas retail loads/stores rc before p (writing rc = o.rc; p = o.p without local r grew to 46 words).
// near miss from lane oc-lbp-20261121 (unreviewed)
