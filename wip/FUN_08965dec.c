/* LBPMATCH module=EBOOT.BIN start=0x08965dec end=0x08965ea0 symbol=FUN_08965dec lang=c99 */
/* Hand-written reconstruction from the disassembly (lane api-lbp-b-20261121, claude-sonnet-5-5); exact match verified; names and types await human review. */
struct Item { unsigned key; int **node; };
struct Self { char pad[236]; unsigned f236; int f240; };
extern int DAT_08be8600;
extern void FUN_089e7ac0(int a, int b, unsigned c, struct Item *items, int *count);
extern void *FUN_089836e8(void *p);
extern void FUN_089d6718(struct Self *self, int *a, unsigned b, void *c, int d);

void FUN_08965dec(struct Self *self)
{
  int n = 1024;
  struct Item items[1024];
  int i;
  FUN_089e7ac0(DAT_08be8600, self->f240, self->f236, items, &n);
  for (i = 0; i < n; i++) {
    int **nd = items[i].node;
    unsigned k = items[i].key;
    int *a = *nd;
    void *r = FUN_089836e8(nd + 2);
    FUN_089d6718(self, a, k, r, 0);
  }
}
// NEAR: 2 words differ (the loads of self+236 (a2) and self+240 (a1) are swapped: retail loads +236 first). Loop and frame match once the node field is read first (it picks the induction pointer).
// Tried: locals f/g in either order, f hoisted above n.
// near miss from lane api-lbp-b-20261121 (unreviewed)
