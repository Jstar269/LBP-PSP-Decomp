/* LBPMATCH module=EBOOT.BIN start=0x0888d4d4 end=0x0888d6a0 symbol=FUN_0888d4d4 lang=c99 */
/* Hand-written reconstruction from the disassembly (lane lbp21o, Claude Opus 5.5); exact match verified; names and types await human review. */
struct Name { int pad[2]; char text[1]; };
struct Node { char pad[0x20]; struct Name *name; };
struct Info {
    char pad0[0xc];
    char f0c[0x4c - 0xc];
    int heartCount;
    int playCount;
    int completionCount;
    float averageRating;
    int yourRating;
    unsigned char shareable;
};
extern struct Node *FUN_0884364c(void *e);
extern struct Node *FUN_088436b0(struct Node *n);
extern const char *FUN_08843570(struct Node *n);
extern int FUN_08ba0d98(const char *a, const char *b);
extern int FUN_088884a4(const char *s);
extern double FUN_08b9cb58(const char *s);
extern float FUN_08b974ac(double d);
extern void FUN_0888d254(struct Node *n, void *dst);
extern char DAT_08baf010[];
extern char DAT_08bb0f18[];
extern char DAT_08bb0ef4[];
extern char DAT_08bb0ed8[];
extern char DAT_08bb0ee4[];
extern char DAT_08bb0f20[];
extern char DAT_08bb0f30[];
extern char DAT_08bafa80[];
extern char DAT_08bafa60[];
extern char DAT_08bafa54[];
int FUN_0888d4d4(void *elem, struct Info *info)
{
    struct Node *n;
    for (n = FUN_0884364c(elem); n != 0; n = FUN_088436b0(n)) {
        const char *name = n->name->text;
        if (FUN_08ba0d98(name, DAT_08baf010) == 0 || FUN_08ba0d98(name, DAT_08bb0f18) == 0)
            continue;
        if (FUN_08ba0d98(name, DAT_08bb0ef4) == 0)
            info->heartCount = FUN_088884a4(FUN_08843570(n));
        else if (FUN_08ba0d98(name, DAT_08bb0ed8) == 0)
            info->playCount = FUN_088884a4(FUN_08843570(n));
        else if (FUN_08ba0d98(name, DAT_08bb0ee4) == 0)
            info->completionCount = FUN_088884a4(FUN_08843570(n));
        else if (FUN_08ba0d98(name, DAT_08bb0f20) == 0)
            info->averageRating = FUN_08b974ac(FUN_08b9cb58(FUN_08843570(n)));
        else if (FUN_08ba0d98(name, DAT_08bb0f30) == 0)
            info->yourRating = FUN_088884a4(FUN_08843570(n));
        else if (FUN_08ba0d98(name, DAT_08bafa80) == 0)
            info->shareable = FUN_088884a4(FUN_08843570(n)) != 0;
        else if (FUN_08ba0d98(name, DAT_08bafa60) != 0 && FUN_08ba0d98(name, DAT_08bafa54) != 0)
            FUN_0888d254(n, info->f0c);
    }
    return 1;
}
