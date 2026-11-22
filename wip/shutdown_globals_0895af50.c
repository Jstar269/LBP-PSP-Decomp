/* LBPMATCH module=EBOOT.BIN start=0x0895af50 end=0x0895afe0 symbol=FUN_0895af50 lang=c99 */
/* Hand-written reconstruction from the disassembly (lane api-lbp-a-20261121, Claude Sonnet 5.5); near miss, NOT exact; names and types await human review. */
extern void FUN_08958ad8(void *);
extern void FUN_08a9e0cc(void *);
extern void FUN_08a9bff8(void *);
extern void FUN_08adbc24(void *, void *, int);
struct Node { struct Node *next; };
struct Big { struct Node *head; char pad[160]; void *f164; };
extern struct Big DAT_08befbb0;
extern int DAT_08befacc;
extern void *DAT_08befb88;
extern char DAT_08befc70[];
extern void *DAT_08be84a0;
void FUN_0895af50(void)
{
    void *h = DAT_08befbb0.f164;
    struct Node *p;
    DAT_08befacc = 0;
    if (h) {
        FUN_08958ad8(h);
        FUN_08a9e0cc(h);
    }
    FUN_08a9e0cc(DAT_08befb88);
    FUN_08a9bff8(DAT_08befc70);
    p = DAT_08befbb0.head;
    while (p != (struct Node *)&DAT_08befbb0) {
        struct Node *next = p->next;
        FUN_08adbc24(DAT_08be84a0, p, 12);
        p = next;
    }
}
// NEAR 31 as counted (36 of 36 words): logic matches. Retail keeps &DAT_08befbb0 in a callee-saved s1 for the whole function (used as base for the +164 member and as the loop sentinel) and a lui base s2 for the later head reload (lw s0,-1104(s2)); ours reloads the address into a temp in the loop test. Pointer local g = &DAT_08befbb0 did not change it.
// near miss from lane api-lbp-a-20261121 (unreviewed)
