/* LBPMATCH module=EBOOT.BIN start=0x0896b138 end=0x0896b21c symbol=FUN_0896b138 lang=c99 */
/* Hand-written reconstruction from the disassembly (lane api-lbp-b-20261121, claude-sonnet-5-5); exact match verified; names and types await human review. */
struct Rect { short x; short y; short w; short h; };
struct Off { unsigned short x; unsigned short y; };
struct Node;
struct VT { int pad[5]; void (*f14)(struct Node *, struct Off *); };
struct Node { const struct VT *vt; int pad; struct Node *next; };
struct Self { char pad[8]; struct Node *first; short x; short y; short w; short h; };

struct Rect FUN_0896b138(struct Self *self, struct Node *stop)
{
  struct Off off;
  struct Node *n = self->first;
  struct Rect r;
  short h, w, y, x;
  off.y = 0;
  off.x = 0;
  h = self->h;
  w = self->w;
  y = self->y;
  x = self->x;
  if (n == 0) {
    off.x = 0;
  } else {
    for (; n != stop; n = n->next)
      n->vt->f14(n, &off);
  }
  r.x = x + off.x;
  r.y = y + off.y;
  r.w = w;
  r.h = h;
  return r;
}
// NEAR: 10 words differ (57/57). Struct and loop match; the 8-byte Rect returns in v0/v1. Callee-saved register assignment is permuted: retail x=s1, y=s4, stop=s5, w=s6, h=s7;
// ours y=s1, stop=s4, x=s5 (the loaded shorts' load order follows from that: retail loads h, w, y with x in the bnez delay slot).
// Tried: load/store orders of the four shorts, int instead of short locals, adds in either order.
// near miss from lane api-lbp-b-20261121 (unreviewed)
