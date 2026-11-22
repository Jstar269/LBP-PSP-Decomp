/* LBPMATCH module=EBOOT.BIN start=0x088e431c end=0x088e441c symbol=FUN_088e431c lang=c++ */
/* Hand-written reconstruction from the disassembly (lane oc-lbp-20261121, Antigravity); near match; names and types await human review. */

struct Node {
    Node *next;
    Node *prev;
    int payload[1];
};

struct Mgr {
    unsigned char pad[0x1e4];
    Node list;
};

struct Net {
    unsigned char pad1[0x24];
    int f24;
    unsigned int f28;
    unsigned char pad2[0x55 - 0x2c];
    unsigned char f55;
};

extern "C" Mgr *DAT_08beed14;
extern "C" void *DAT_08beee0c;
extern "C" const int DAT_08bb440c;
extern "C" const unsigned int DAT_08bb4410;
extern "C" int FUN_088e3e74(Net *, int, int, int);
extern "C" void FUN_088f3b0c(void *, void *);
extern "C" void FUN_088e68a0(void *, unsigned char, int);

extern "C" void FUN_088e431c(Net *param_1, int param_2)
{
    if (FUN_088e3e74(param_1, 20, param_2, DAT_08bb440c) != 0) {
        unsigned int page_sz = DAT_08bb4410;
        param_1->f28 = page_sz;
        int skip = (param_1->f24 - 1) * (int)page_sz;
        Node *n = DAT_08beed14->list.next;
        Node *end = &DAT_08beed14->list;
        while (skip > 0 && n != end) {
            skip--;
            n = n->next;
        }
        unsigned int sent = 0;
        while (n != &DAT_08beed14->list) {
            sent++;
            if (sent > param_1->f28) {
                break;
            }
            FUN_088f3b0c(DAT_08beee0c, n->payload);
            n = n->next;
        }
        if (sent == 0) {
            param_1->f55 = 1;
            FUN_088e68a0(DAT_08beee0c, 1, 24);
        }
    }
}

// Near-miss note: Register/scheduling diff on 64/64 words.
// near miss from lane oc-lbp-20261121 (unreviewed)
