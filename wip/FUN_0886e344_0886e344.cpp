/* LBPMATCH module=EBOOT.BIN start=0x0886e344 end=0x0886e408 symbol=FUN_0886e344 lang=c++ */
/* Hand-written reconstruction from the disassembly (lane oc-lbp-20261121, Antigravity); near match; names and types await human review. */

extern char DAT_08bb045c[];
extern int DAT_08bae290;

extern "C" void *FUN_08a9e838(unsigned);
extern "C" void FUN_088451b8(void *, const char *);
extern "C" void FUN_08878ab8(int, char *);
extern "C" void FUN_08843f04(void *, int);
extern "C" unsigned FUN_08ba0e8c(const char *);
extern "C" void FUN_08842ef4(void *, const char *, unsigned);
extern "C" void FUN_08844248(void *, void *);

struct Sub {
    unsigned char pad[0x14];
    int hash;
};

struct Obj {
    unsigned char pad[0x44];
    Sub *sub;
};

struct Str {
    int rep[3];
    Str(const char *s) {
        FUN_08842ef4(this, s, FUN_08ba0e8c(s));
    }
};

struct Node2 {
    void *vt;
    unsigned char pad[28];
    Str str;
    unsigned char flag;
};

inline void *operator new(unsigned, void *p) throw() { return p; }

extern "C" void FUN_0886e344(Obj *self, void *param_2)
{
    char buf[41];
    void *p1 = FUN_08a9e838(80);
    FUN_088451b8(p1, DAT_08bb045c);
    FUN_08878ab8(self->sub->hash, buf);
    Node2 *p2 = (Node2 *)FUN_08a9e838(48);
    FUN_08843f04(p2, 4);
    p2->vt = &DAT_08bae290;
    new (&p2->str) Str(buf);
    p2->flag = 0;
    FUN_08844248(p1, p2);
    FUN_08844248(param_2, p1);
}
// 24 differing words (51/49): twin of 0x0886cf3c; same s1 allocation for &p2->str vs placement-new null check.
// near miss from lane oc-lbp-20261121 (unreviewed)
