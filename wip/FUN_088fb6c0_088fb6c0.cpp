/* LBPMATCH module=EBOOT.BIN start=0x088fb6c0 end=0x088fb778 symbol=FUN_088fb6c0 lang=c++ */
/* Hand-written reconstruction from the disassembly (lane oc-lbp-20261121, Antigravity); near match; names and types await human review. */

extern "C" void FUN_08b8829c(void);
extern "C" void *FUN_08a9e838(unsigned int);

inline void *operator new(unsigned int, void *p) throw() { return p; }

struct Vec {
    void **begin;
    void **end;
    void **cap;

    inline void **do_copy(void **first, void **last, void **result) {
        void **cur = result;
        for (; first != last; ++first, ++cur) {
            new (cur) void *(*first);
        }
        return cur;
    }
};

extern "C" void FUN_088fb6c0(Vec *param_1, const Vec *param_2)
{
    unsigned int n = (unsigned int)(param_2->end - param_2->begin);
    param_1->begin = 0;
    param_1->end = 0;
    param_1->cap = 0;
    if (n > 0x3fffffffU) {
        FUN_08b8829c();
    }
    void **buf = (void **)FUN_08a9e838(n * sizeof(void *));
    param_1->begin = buf;
    param_1->end = buf;
    param_1->cap = buf + n;
    param_1->end = param_1->do_copy(param_2->begin, param_2->end, param_1->begin);
}

// Near-miss note: 46/46 words, 11 differ: std::vector copy loop register choice (a0/v1/a2 vs v1/a1/a0).
// near miss from lane oc-lbp-20261121 (unreviewed)
