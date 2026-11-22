/* LBPMATCH module=EBOOT.BIN start=0x088f8d78 end=0x088f8e30 symbol=FUN_088f8d78 lang=c++ */
/* Hand-written reconstruction from the disassembly (lane oc-lbp-20261121, Antigravity); exact match verified; names and types await human review. */

struct Item {
    int pad[14];
    int f38;
};

struct Node {
    Node *next;
    Node *prev;
    Item *val;
};

extern "C" void *DAT_08beee0c;
extern "C" void FUN_088f87ec(Node *);
extern "C" void FUN_088f8aac(Node *);
extern "C" void *FUN_08a9e824(unsigned int);
extern "C" void FUN_08a9e0c4(void *);
extern "C" void FUN_088f4a64(void *, int *, int);

extern "C" void FUN_088f8d78(Node *param_1)
{
    FUN_088f87ec(param_1);
    FUN_088f8aac(param_1);
    int count = 0;
    for (Node *n = param_1->next; n != param_1; n = n->next) {
        count++;
    }
    int *buf = (int *)FUN_08a9e824(count * 4);
    int i = 0;
    for (Node *n = param_1->next; n != param_1; n = n->next) {
        buf[i] = n->val->f38;
        i++;
    }
    FUN_088f4a64(DAT_08beee0c, buf, count);
    if (buf != 0) {
        FUN_08a9e0c4(buf);
    }
}
