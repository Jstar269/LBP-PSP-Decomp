/* LBPMATCH module=EBOOT.BIN start=0x0888cf30 end=0x0888cff0 symbol=FUN_0888cf30 lang=c99 */
/* Hand-written reconstruction from the disassembly (lane lbp17b, Codex GPT-6); exact match verified; names and types await human review. */

typedef struct Node Node;
struct Node { unsigned char pad[0x14]; char *name; char *value; };
extern void FUN_08b9eb88(void *, int, unsigned int);
extern int FUN_08ba0d98(const char *, const char *);
extern int FUN_08843824(Node *);
extern void FUN_0888cddc(const char *, int);
extern char DAT_08bafa48[];

int FUN_0888cf30(const char *param_1, Node *param_2, char *param_3, int param_4)
{
    FUN_08b9eb88(param_3, 0, 0x14);
    while (*param_1 != 0)
        *param_3++ = *param_1++;
    while (param_2 != 0) {
        if (FUN_08ba0d98(param_2->name + 8, DAT_08bafa48) == 0) {
            FUN_0888cddc(param_2->value + 8, param_4);
            break;
        }
        param_2 = (Node *)FUN_08843824(param_2);
    }
    return 1;
}
