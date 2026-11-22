/* LBPMATCH module=EBOOT.BIN start=0x0886cf3c end=0x0886d008 symbol=FUN_0886cf3c lang=c++ */
/* Hand-written reconstruction from the disassembly (lane oc-lbp-20261121, Antigravity); near match; names and types await human review. */

extern char DAT_08bb0430[];
extern char DAT_08baef90[];
extern int DAT_08bae290;

extern "C" void *FUN_08a9e838(unsigned);
extern "C" void FUN_088451b8(void *, const char *);
extern "C" int FUN_08ba0868(char *, int, const char *, ...);
extern "C" void FUN_08843f04(void *, int);
extern "C" unsigned FUN_08ba0e8c(const char *);
extern "C" void FUN_08842ef4(void *, const char *, unsigned);
extern "C" void FUN_08844248(void *, void *);

struct Obj {
    unsigned char pad[0x3c];
    unsigned val;
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

extern "C" void FUN_0886cf3c(Obj *self, void *param_2)
{
    char buf[64];
    void *p1 = FUN_08a9e838(80);
    FUN_088451b8(p1, DAT_08bb0430);
    FUN_08ba0868(buf, 64, DAT_08baef90, self->val);
    Node2 *p2 = (Node2 *)FUN_08a9e838(48);
    FUN_08843f04(p2, 4);
    p2->vt = &DAT_08bae290;
    new (&p2->str) Str(buf);
    p2->flag = 0;
    FUN_08844248(p1, p2);
    FUN_08844248(param_2, p1);
}
// 26 differing words (53/51): retail allocates callee-saved s1 for &p2->str (96-byte frame) without a null check;
// with throw() placement-new we get the 96-byte frame and s1, plus a 2-word null check; without it we drop to an 80-byte frame (47 differ).
// near miss from lane oc-lbp-20261121 (unreviewed)
