/* LBPMATCH module=EBOOT.BIN start=0x088686bc end=0x0886875c symbol=FUN_088686bc lang=c++ */
/* Hand-written reconstruction from the disassembly (lane oc-lbp-20261121, Antigravity); exact match verified; names and types await human review. */

extern int DAT_08bb0038;
extern char DAT_08bb00cc[];

extern "C" void FUN_0885d290(void *, int, int, int);
extern "C" void *FUN_08b9eb88(void *, int, unsigned);
extern "C" char *FUN_08887624(void);
extern "C" int FUN_08ba0868(char *, int, const char *, ...);
extern "C" void FUN_0885d244(void *, char *);

struct Node {
    int color;
    Node *parent;
    Node *left;
    Node *right;
};

struct Tree {
    int pad;
    Node header;
    int count;
};

struct Obj {
    void *vt;
    unsigned char pad[204];
    Tree tree;
};

extern "C" void FUN_088686bc(Obj *self)
{
    char buf[256];
    FUN_0885d290(self, 3, 0, 5);
    self->vt = &DAT_08bb0038;
    Tree *t = &self->tree;
    Node *hdr = &t->header;
    FUN_08b9eb88(hdr, 0, 16);
    t->header.color = 0;
    t->header.parent = 0;
    t->header.left = hdr;
    t->header.right = hdr;
    t->count = 0;
    char *s = FUN_08887624();
    FUN_08ba0868(buf, 256, DAT_08bb00cc, s);
    FUN_0885d244(self, buf);
}
