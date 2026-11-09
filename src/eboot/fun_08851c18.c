/* LBPMATCH module=EBOOT.BIN start=0x08851c18 end=0x08851cd0 symbol=FUN_08851c18 lang=c99 */
/* Hand-written reconstruction from the disassembly (lane lbp16b, GPT-6); exact match verified; names and types await human review. */
typedef void (*Release)(int *);
extern char DAT_08baee88[];
extern char DAT_08bef8f4;
extern void FUN_08851894(char *, unsigned int *, unsigned int);
extern unsigned int *FUN_08a9e838(unsigned int);
extern void FUN_0894a258(unsigned int *);
extern void FUN_0884ab18(char *, int **);
extern void FUN_08ad3088(void *);
extern void FUN_08ad2810(void *);

void FUN_08851c18(char *param_1, unsigned int *param_2, unsigned int param_3)
{
    unsigned int *created;
    int *local[4];
    int *object;

    FUN_08851894(param_1, param_2, param_3);
    created = FUN_08a9e838(16);
    FUN_0894a258(created);
    created[1] = 1;
    created[0] = (unsigned int)DAT_08baee88;
    created[2] = 0;
    created[3] = 0;
    local[0] = (int *)created;
    FUN_0884ab18(param_1, local);
    object = local[0];
    if (object != 0) {
        int count;
        FUN_08ad3088(&DAT_08bef8f4);
        count = object[1] - 1;
        object[1] = count;
        FUN_08ad2810(&DAT_08bef8f4);
        if (count == 0) {
            ((Release *)*(void **)object)[1](object);
        }
    }
}
