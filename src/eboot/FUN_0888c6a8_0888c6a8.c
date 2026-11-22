/* LBPMATCH module=EBOOT.BIN start=0x0888c6a8 end=0x0888c75c symbol=FUN_0888c6a8 lang=c99 */
/* Hand-written reconstruction from the disassembly (lane oc-lbp-20261121, Antigravity); exact match verified; names and types await human review. */

extern char DAT_08bb0f00[];
extern char DAT_08bb0f08[];

struct Str {
    int pad[2];
    char str[1];
};

struct Node {
    unsigned char pad[32];
    struct Str *name;
};

struct Out {
    int a;
    int b;
};

extern struct Node *FUN_0884364c(void *);
extern struct Node *FUN_088436b0(struct Node *);
extern int FUN_08ba0d98(const char *, const char *);
extern void *FUN_08843570(struct Node *);
extern int FUN_088884a4(void *);

int FUN_0888c6a8(void *param_1, struct Out *param_2)
{
    struct Node *node;
    for (node = FUN_0884364c(param_1); node != 0; node = FUN_088436b0(node)) {
        const char *name = node->name->str;
        if (FUN_08ba0d98(name, DAT_08bb0f00) == 0) {
            param_2->a = FUN_088884a4(FUN_08843570(node));
        } else if (FUN_08ba0d98(name, DAT_08bb0f08) == 0) {
            param_2->b = FUN_088884a4(FUN_08843570(node));
        }
    }
    return 1;
}
