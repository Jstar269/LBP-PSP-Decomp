/* LBPMATCH module=EBOOT.BIN start=0x08947a88 end=0x08947b34 symbol=FUN_08947a88 lang=c99 */
/* Hand-written reconstruction from the disassembly (lane api-lbp-a-20261121, Claude Sonnet 5.5); near miss, NOT exact; names and types await human review. */
extern void *FUN_08b9eb88(void *, int, int);
extern void FUN_08baa4fc(void *, void *);
extern void FUN_08ba0868(char *, int, const char *);
extern const char *DAT_08bef8d8;
struct Src { char pad[20]; int f20; char name[1]; };
struct SrcP { struct Src *s; };
struct O {
    int pad0;
    int size;
    int a8, a12;
    int f16, f20, f24, f28;
    char pad1[20];
    int f52;
    char pad2[8];
    char title[16];
    char name[16];
    char pad3[1428];
    int f1524, f1528;
};
void FUN_08947a88(struct O *o, struct SrcP *p)
{
    FUN_08b9eb88(&o->size, 0, 1536);
    o->size = 1536;
    FUN_08baa4fc(&o->a8, &o->a12);
    o->f28 = 31;
    o->f16 = 32;
    o->f24 = 33;
    o->f20 = 34;
    FUN_08ba0868(o->title, 13, DAT_08bef8d8);
    FUN_08ba0868(o->name, 20, (const char *)p->s + 24);
    o->f52 = 21;
    o->f1524 = p->s->f20;
}
// NEAR 3 words: same as wip/init_view_089477cc.c (size store in the jal delay slot in retail).
// near miss from lane api-lbp-a-20261121 (unreviewed)
