/* LBPMATCH module=EBOOT.BIN start=0x0891e224 end=0x0891e2ec symbol=FUN_0891e224 lang=c99 */
/* Hand-written reconstruction from the disassembly (lane lbp18c, GPT-6); exact match verified; names and types await human review. */

typedef struct TreeNode {
    void **vtable;
    int references;
    int pad08;
    int pad0c;
    unsigned int key;
} TreeNode;

typedef struct InsertArgs {
    unsigned int key;
    TreeNode *object;
} InsertArgs;

extern TreeNode *FUN_0891d928(char *, unsigned int *);
extern TreeNode *FUN_0891e078(char *, TreeNode *, InsertArgs *);
extern void FUN_08ad3088(void *);
extern void FUN_08ad2810(void *);
extern char DAT_08bef8f4;

int FUN_0891e224(char *param_1, unsigned int *param_2)
{
    TreeNode *result;
    TreeNode *object;
    InsertArgs args;
    int references;

    result = FUN_0891d928(param_1, param_2);
    if (result == (TreeNode *)(param_1 + 4) || *param_2 < result->key) {
        args.key = *param_2;
        args.object = 0;
        result = FUN_0891e078(param_1, result, &args);
        object = args.object;
        if (object != 0) {
            FUN_08ad3088(&DAT_08bef8f4);
            references = object->references - 1;
            object->references = references;
            FUN_08ad2810(&DAT_08bef8f4);
            if (references == 0) {
                ((void (*)(TreeNode *))object->vtable[2])(object);
            }
        }
    }
    return (int)((char *)result + 0x14);
}
