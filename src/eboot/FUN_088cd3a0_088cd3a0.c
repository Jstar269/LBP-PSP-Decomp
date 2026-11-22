/* LBPMATCH module=EBOOT.BIN start=0x088cd3a0 end=0x088cd494 symbol=FUN_088cd3a0 lang=c99 */
/* Hand-written reconstruction from the disassembly (lane oc-lbp-20261121, Antigravity); exact match verified; names and types await human review. */

struct Obj {
    unsigned char pad1[0x3c];
    unsigned char count;
    unsigned char pad2[0x50 - 0x3d];
    unsigned int arr[250];
};

struct Query {
    int type;
    unsigned int id;
};

struct Msg {
    unsigned int id;
    unsigned char flag;
    unsigned char pad[35];
};

extern void *DAT_08beed14;
extern void *DAT_08beee0c;
extern int FUN_08895658(void *, struct Query *);
extern unsigned int FUN_088d7a84(void *, unsigned int);
extern void FUN_088e666c(void *, int, struct Msg *);

unsigned int FUN_088cd3a0(struct Obj *self, void *param_2, unsigned int param_3)
{
    unsigned int sent = 0;
    short i;

    if (param_3 == 0) {
        return 0;
    }
    for (i = 0; i < self->count; i++) {
        struct Query q;
        q.type = 1;
        q.id = self->arr[i];
        if (!FUN_08895658(param_2, &q) && FUN_088d7a84(DAT_08beed14, self->arr[i]) == 0) {
            struct Msg m;
            m.id = self->arr[i];
            m.flag = 1;
            sent++;
            FUN_088e666c(DAT_08beee0c, 15, &m);
            if (sent >= param_3) {
                break;
            }
        }
    }
    return sent;
}
