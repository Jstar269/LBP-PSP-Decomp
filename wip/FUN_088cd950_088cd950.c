/* LBPMATCH module=EBOOT.BIN start=0x088cd950 end=0x088cda1c symbol=FUN_088cd950 lang=c99 */
/* Hand-written reconstruction from the disassembly (lane oc-lbp-20261121, Antigravity); near match; names and types await human review. */

struct Obj {
    unsigned char pad1[0x3c];
    unsigned char count;
    unsigned char pad2[0x50 - 0x3d];
    int arr[250];
};

struct Node {
    struct Node *next;
    struct Node *prev;
    int f8;
    int val;
};

struct ListHolder {
    int pad[5];
    struct Node list;
};

struct Msg {
    int val;
    unsigned char flag;
    unsigned char pad[27];
};

extern void *DAT_08beee0c;
extern void FUN_088e666c(void *, int, struct Msg *);

unsigned int FUN_088cd950(struct Obj *self, struct ListHolder *param_2, unsigned int param_3)
{
    unsigned int sent = 0;

    if (param_3 != 0) {
        struct Node *n = param_2->list.next;
        struct Node *end = &param_2->list;
        while (n != end) {
            int val = n->val;
            short i;
            for (i = 0; i < self->count; i++) {
                if (self->arr[i] == val) {
                    break;
                }
            }
            if (i >= self->count) {
                struct Msg m;
                m.val = val;
                m.flag = 0;
                sent++;
                FUN_088e666c(DAT_08beee0c, 15, &m);
                if (sent >= param_3) {
                    break;
                }
            }
            n = n->next;
        }
    }
    return sent;
}

// Near match: 49/51 words; 28 differ; first at +0x24.
// Frame size (64 bytes), inner loop, and Msg layout match; remaining diffs are
// outer loop block placement (retail jumps to send block after inner search, and
// places list-end bnel before the send block).
// Ruled out: single for(n!=end && sent<param_3) loop (47/51), nested do/while (47/51).
// near miss from lane oc-lbp-20261121 (unreviewed)
