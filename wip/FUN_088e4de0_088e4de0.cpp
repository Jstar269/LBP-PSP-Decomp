/* LBPMATCH module=EBOOT.BIN start=0x088e4de0 end=0x088e4e9c symbol=FUN_088e4de0 lang=c++ */
/* Hand-written reconstruction from the disassembly (lane oc-lbp-20261121, Antigravity); near match; names and types await human review. */

struct Key {
    int w[5];
};

struct Node {
    unsigned char pad[0x3c];
    Key key;
};

struct Net {
    unsigned char pad[0x28];
    int f28;
};

extern "C" void *DAT_08beee0c;
extern "C" const int DAT_08bb440c;
extern "C" const int DAT_08bb4410;
extern "C" Node *FUN_088e4b84(Net *, void *);
extern "C" int FUN_088e3e74(Net *, int, int, int);
extern "C" void FUN_088f3b0c(void *, Key *);

extern "C" void FUN_088e4de0(Net *param_1, void *param_2, int param_3)
{
    Node *n = FUN_088e4b84(param_1, param_2);
    if (FUN_088e3e74(param_1, 18, param_3, DAT_08bb440c) != 0) {
        void *ui = DAT_08beee0c;
        param_1->f28 = DAT_08bb4410;
        Key k = n->key;
        FUN_088f3b0c(ui, &k);
        return;
    }
    FUN_088e4b84(param_1, 0);
}

// Near-miss note: Matches first 15 words with const int rodata; sibling tail-call suppressed by &k local.
// near miss from lane oc-lbp-20261121 (unreviewed)
