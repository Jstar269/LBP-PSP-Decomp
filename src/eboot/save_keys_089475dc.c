/* LBPMATCH module=EBOOT.BIN start=0x089475dc end=0x089476bc symbol=FUN_089475dc lang=c99 */
/* Hand-written reconstruction from the disassembly (lane api-lbp-a-20261121, Claude Sonnet 5.5); exact match verified; names and types await human review. */
extern void FUN_08ad3088(void *);
extern void FUN_08ad2810(void *);
extern void FUN_08ba0868(char *, int, const char *, const char *, int, const char *);
extern int FUN_08ba9fa4(char *, void *);
extern void FUN_08adbe84(void *, void *, int);
struct Info { int w0[4]; char k1[16]; int pad[4]; char k2[16]; int pad2[6]; };
struct K { char b[16]; };
struct S { char pad[1556]; char lock[128]; char pad2[32]; struct K k1[2]; struct K k2[2]; };
extern struct S *DAT_08bedb1c;
extern const char *DAT_08bdb7fc;
extern int DAT_08bef8d8;
extern const char DAT_08bb765c[];
extern const char DAT_08bb7668[];
int FUN_089475dc(int idx)
{
    struct S *s = DAT_08bedb1c;
    struct Info info;
    char buf[264];
    int ok;
    FUN_08ad3088(s->lock);
    FUN_08ba0868(buf, 256, DAT_08bb765c, DAT_08bdb7fc, DAT_08bef8d8, DAT_08bb7668 + idx * 20);
    ok = 0;
    if (FUN_08ba9fa4(buf, &info) == 0) {
        ok = 1;
        FUN_08adbe84(s->k1[idx].b, info.k1, 16);
        FUN_08adbe84(s->k2[idx].b, info.k2, 16);
    }
    FUN_08ad2810(s->lock);
    return ok;
}
