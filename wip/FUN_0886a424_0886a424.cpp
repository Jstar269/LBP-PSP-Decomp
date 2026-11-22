/* LBPMATCH module=EBOOT.BIN start=0x0886a424 end=0x0886a4f8 symbol=FUN_0886a424 lang=c++ */
/* Hand-written reconstruction from the disassembly (lane oc-lbp-20261121, Antigravity); near match; names and types await human review. */

struct Mutex { int a; };
extern Mutex DAT_08bef8f4;

struct Ref {
    void (**vt)(Ref *);
    int count;
};

struct Key {
    int f0;
    Ref *ref;
    int f8;
};

struct Entry {
    Key k;
    int val;
};

struct Pair {
    void *it;
    bool ok;
};

struct Pair2 {
    void *it;
    bool ok;
    Pair2(const Pair &o) : it(o.it), ok(o.ok) {}
};

extern "C" void FUN_0884d5f4(Key *, const Key *);
extern "C" Pair FUN_0886a2dc(void *, Entry *);
extern "C" void FUN_08ad3088(void *);
extern "C" void FUN_08ad2810(void *);

struct Obj {
    unsigned char pad1[200];
    int tree;
    unsigned char pad2[456 - 204];
    int count;
};

extern "C" int FUN_0886a424(Obj *self, const Key *key, int val)
{
    Entry e;
    FUN_0884d5f4(&e.k, key);
    e.val = val;
    Pair tmp = FUN_0886a2dc(&self->tree, &e);
    Pair2 p(tmp);
    bool ok = p.ok;
    Ref *r = e.k.ref;
    if (r != 0) {
        FUN_08ad3088(&DAT_08bef8f4);
        int c = --r->count;
        FUN_08ad2810(&DAT_08bef8f4);
        if (c == 0) {
            r->vt[2](r);
        }
    }
    if (ok) {
        self->count++;
    }
    return 1;
}
// 35 differing words (52/53): prologue, calls, ref-release, and epilogue all match;
// differs by 1 word in copying the 8-byte pair<it,bool> return (retail stores v1 to 36(sp),
// reloads lbu v1,36(sp) and sb v1,4(sp); ours copies both words with sw v1,4(sp) and lbu s3,4(sp)).
// near miss from lane oc-lbp-20261121 (unreviewed)
