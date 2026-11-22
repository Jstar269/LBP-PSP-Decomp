/* LBPMATCH module=EBOOT.BIN start=0x088e8494 end=0x088e8554 symbol=FUN_088e8494 lang=c++ */
/* Hand-written reconstruction from the disassembly (lane oc-lbp-20261121, Antigravity); exact match verified; names and types await human review. */

struct Node {
    Node *next;
    Node *prev;
    void *val;
};

struct Ui {
    unsigned char pad1[0x5c];
    unsigned int f5c;
    unsigned char pad2[0xc4 - 0x60];
    Node list;
    unsigned int pad_bits : 9;
    unsigned int b9 : 1;
    unsigned int rest : 22;
};

extern "C" void *DAT_08be84a0;
extern "C" void FUN_088d6674(void *, unsigned int);
extern "C" void FUN_08b88fc8(Node *);
extern "C" void FUN_08adbc24(void *, Node *, int);

extern "C" void FUN_088e8494(Ui *param_1)
{
    Node *list = &param_1->list;
    if (list->next != list) {
        FUN_088d6674(list->prev->val, param_1->f5c);
        Node *last = list->prev;
        FUN_08b88fc8(last);
        FUN_08adbc24(DAT_08be84a0, last, 12);
        for (Node *n = list->next; n != list; n = n->next) {
            FUN_088d6674(n->val, 0xffff);
        }
        Node *n = list->next;
        while (n != list) {
            Node *next = n->next;
            FUN_08adbc24(DAT_08be84a0, n, 12);
            n = next;
        }
        list->next = list;
        list->prev = list;
    }
    param_1->b9 = 0;
}
