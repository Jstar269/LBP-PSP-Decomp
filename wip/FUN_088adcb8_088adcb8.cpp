/* LBPMATCH module=EBOOT.BIN start=0x088adcb8 end=0x088adda8 symbol=FUN_088adcb8 lang=c++ */
/* Hand-written reconstruction from the disassembly (lane oc-lbp-20261121, Antigravity); exact match verified; names and types await human review. */

inline void *operator new(unsigned, void *p) throw() { return p; }

extern "C" void *DAT_08be84a0;

extern "C" void FUN_08b88fc8(void *);
extern "C" void FUN_08adbc24(void *, void *, int);
extern "C" void *FUN_08adbb84(void *, int);
extern "C" void FUN_08b88fac(void *, void *);

struct Node {
    Node *next;
    Node *prev;
    unsigned short val;
};

struct List {
    Node *next;
    Node *prev;

    inline void erase_range(Node *first, Node *last)
    {
        while (first != last) {
            Node *n = first->next;
            FUN_08b88fc8(first);
            FUN_08adbc24(DAT_08be84a0, first, 12);
            first = n;
        }
    }

    inline void insert_range(Node *pos, Node *first, Node *last)
    {
        for (; first != last; first = first->next) {
            Node *n = (Node *)FUN_08adbb84(DAT_08be84a0, 12);
            new (&n->val) unsigned short(first->val);
            FUN_08b88fac(n, pos);
        }
    }

    inline List &assign(const List &src)
    {
        if (this != &src) {
            Node *first1 = next;
            Node *last1 = (Node *)this;
            Node *first2 = src.next;
            Node *last2 = (Node *)&src;
            while (first1 != last1 && first2 != last2) {
                first1->val = first2->val;
                first1 = first1->next;
                first2 = first2->next;
            }
            if (first2 == last2) {
                erase_range(first1, last1);
            } else {
                insert_range(last1, first2, last2);
            }
        }
        return *this;
    }
};

extern "C" List &FUN_088adcb8(List &dst, const List &src)
{
    return dst.assign(src);
}

// Remaining diffs (61/60 words; 61 differ): GCC spills s3 (sp,-32) whereas retail only uses s0..s2 (sp,-16) by keeping first1/first2 in a0/v1 during the copy loop and reusing s1/s0 in erase/insert.
// near miss from lane oc-lbp-20261121 (unreviewed)
