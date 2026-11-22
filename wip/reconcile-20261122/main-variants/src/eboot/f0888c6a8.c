/* LBPMATCH module=EBOOT.BIN start=0x0888c6a8 end=0x0888c75c symbol=FUN_0888c6a8 lang=c99 */
/* Hand-written reconstruction from the disassembly (lane lbp17b, Codex GPT-6); exact match verified; names and types await human review. */

typedef struct NamedItem NamedItem;
typedef struct ListEntry ListEntry;
struct NamedItem { unsigned char pad[8]; unsigned char name[1]; };
struct ListEntry { unsigned char pad[0x20]; NamedItem *item; };
extern char DAT_08bb0f00[];
extern char DAT_08bb0f08[];
extern ListEntry *FUN_0884364c(void *);
extern ListEntry *FUN_088436b0(ListEntry *);
extern int FUN_08ba0d98(const char *, const char *);
extern void *FUN_08843570(ListEntry *);
extern unsigned int FUN_088884a4(void *);

int FUN_0888c6a8(void *param_1, unsigned int *param_2)
{
    ListEntry *entry = FUN_0884364c(param_1);
    for (; entry != 0; entry = FUN_088436b0(entry)) {
        const char *name = entry->item->name;
        if (FUN_08ba0d98(name, DAT_08bb0f00) == 0) {
            param_2[0] = FUN_088884a4(FUN_08843570(entry));
        } else if (FUN_08ba0d98(name, DAT_08bb0f08) == 0) {
            param_2[1] = FUN_088884a4(FUN_08843570(entry));
        }
    }
    return 1;
}
