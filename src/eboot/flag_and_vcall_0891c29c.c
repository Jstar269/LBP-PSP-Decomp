/* LBPMATCH module=EBOOT.BIN start=0x0891c29c end=0x0891c310 symbol=FUN_0891c29c lang=c99 */
/* Clone of src/eboot/flag_and_vcall_0891c124.c (lbpclone.py tag d1): addresses substituted; exact match verified; names and types await human review. */
/* Hand-written reconstruction from the disassembly (lane fb-near1, Claude); names and types await human review. */
typedef struct Obj Obj;
typedef struct Other Other;

struct Other { void (**vt)(Other *, int *); };
struct Obj { void (**vt)(Obj *); char pad[12]; unsigned char flag; char pad2[151]; Other *child; };

int FUN_0891c29c(Obj *p, int a, int *out, int b, unsigned char c)
{
  if (c) {
    p->child->vt[3](p->child, out);
    p->flag = 1;
    *out = 0;
  } else {
    p->vt[5](p);
  }
  return 1;
}
