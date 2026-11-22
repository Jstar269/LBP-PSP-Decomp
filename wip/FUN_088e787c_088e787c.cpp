/* LBPMATCH module=EBOOT.BIN start=0x088e787c end=0x088e7930 symbol=FUN_088e787c lang=c++ */
/* Hand-written reconstruction from the disassembly (lane oc-lbp-20261121, Antigravity); near match; names and types await human review. */

struct Node {
    Node *next;
    Node *prev;
    void *val;
};

extern "C" bool FUN_088e7840(unsigned char *, void *, void *);
extern "C" void FUN_08b88f70(Node *, Node *, Node *);

extern "C" void FUN_088e787c(Node *param_1, Node *param_2, unsigned char param_3)
{
    if (param_1 != param_2) {
        Node *first1 = param_1->next;
        Node *first2 = param_2->next;
        unsigned char cmp = param_3;
        while (first1 != param_1) {
            if (first2 == param_2) {
                return;
            }
            if (FUN_088e7840(&cmp, first2->val, first1->val)) {
                Node *next = first2->next;
                FUN_08b88f70(first1, first2, next);
                first2 = next;
            } else {
                first1 = first1->next;
            }
        }
        if (first2 != param_2) {
            FUN_08b88f70(param_1, first2, param_2);
        }
    }
}

// Near-miss note: std::list::merge clone of 0x088e1e70 with unsigned char comparator.
// near miss from lane oc-lbp-20261121 (unreviewed)
