/* LBPMATCH module=EBOOT.BIN start=0x088e28e8 end=0x088e29c4 symbol=FUN_088e28e8 lang=c++ */
/* Hand-written reconstruction from the disassembly (lane oc-lbp-20261121, Antigravity); near match; names and types await human review. */

struct Node {
    Node *next;
    Node *prev;
    void *val;
};

struct Holder {
    void *pool;
    void **arr;
    unsigned int cap;
    unsigned int count;
};

struct Mgr {
    unsigned char pad[0x188];
    Holder holder;
};

struct Req {
    int f0;
    int pad[3];
    unsigned int count;
    Node list;
};

inline void *operator new(unsigned int, void *p) throw()
{
    return p;
}

extern "C" Mgr *DAT_08beed14;
extern "C" void *DAT_08be84a0;
extern "C" void *FUN_08adbb84(void *, unsigned int);
extern "C" void FUN_08b88fac(Node *, Node *);
extern "C" void FUN_088e2738(Node *, int);
extern "C" void FUN_088e1c24(Req *, int);

extern "C" bool FUN_088e28e8(void *param_1, Req *param_2)
{
    (void)param_1;
    Node *list = &param_2->list;
    if (list->next == list) {
        Holder *h = &DAT_08beed14->holder;
        for (unsigned int i = 0; i < h->count; i++) {
            void *item = h->arr[i];
            Node *n = (Node *)FUN_08adbb84(DAT_08be84a0, sizeof(Node));
            new (&n->val) void *(item);
            FUN_08b88fac(n, list);
        }
        FUN_088e2738(list, param_2->f0);
        param_2->count = h->count;
    }
    FUN_088e1c24(param_2, 0);
    return list->next != list;
}

// Near-miss note: Loop/regalloc diff on 57/55 words.
// near miss from lane oc-lbp-20261121 (unreviewed)
