/* LBPMATCH module=EBOOT.BIN start=0x088bcc10 end=0x088bcce4 symbol=FUN_088bcc10 lang=c++ */
/* Hand-written reconstruction from the disassembly (lane oc-lbp-20261121, Antigravity); exact match verified; names and types await human review. */

extern "C" void *DAT_08be84a0;
extern "C" void FUN_088bcb3c(void *);
extern "C" void FUN_088bc32c(void);
extern "C" void FUN_088bb970(void);
extern "C" void FUN_0894ebcc(void);
extern "C" void FUN_08b88fc8(void *);
extern "C" void FUN_088050b8(void *);
extern "C" void FUN_08adbc24(void *, void *, int);
extern "C" void FUN_088bc478(void *, int);

struct Sub {
    void (**vt)(Sub *, int, int, void (*)(void), void *, void (*)(void), void (*)(void));
};

struct Item {
    unsigned char pad[0xa8];
    Sub *sub;
};

struct NodeBase {
    NodeBase *next;
    NodeBase *prev;
};

struct Node : NodeBase {
    Item *item;
};

struct Obj {
    unsigned char pad1[0x3c];
    NodeBase list;
    int f44;
    unsigned char pad2[0x60 - 0x48];
    int f60;
    unsigned char pad3[0x80 - 0x64];
    int f80;
};

extern "C" void FUN_088bcc10(Obj *self)
{
    FUN_088bcb3c(&self->f60);
    if (self->list.next != &self->list) {
        Sub *sub = ((Node *)self->list.next)->item->sub;
        sub->vt[10](sub, 0, 0, FUN_088bc32c, self, FUN_088bb970, FUN_0894ebcc);
        Node *n = (Node *)self->list.next;
        FUN_08b88fc8(n);
        if (n->item != 0) {
            FUN_088050b8(n->item);
        }
        FUN_08adbc24(DAT_08be84a0, n, 12);
        FUN_088bc478(self, self->f44 + 1);
    } else {
        self->f80++;
        FUN_088bc478(self, self->f44 + 2);
    }
}
