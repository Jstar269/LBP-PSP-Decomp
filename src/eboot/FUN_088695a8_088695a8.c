/* LBPMATCH module=EBOOT.BIN start=0x088695a8 end=0x08869688 symbol=FUN_088695a8 lang=c99 */
/* Hand-written reconstruction from the disassembly (lane oc-lbp-20261121, Antigravity); exact match verified; names and types await human review. */

extern char DAT_08baf1d8[];
extern int DAT_08bef8f4;

extern void FUN_08878ab8(int, char *);
extern char *FUN_08887624(void);
extern int FUN_08ba0868(char *, int, const char *, ...);
extern void FUN_0885d244(void *, char *);
extern void FUN_0885cc04(void *, void *, void *);
extern void FUN_08ad3088(int *);
extern void FUN_08ad2810(int *);

struct Ref {
    void (**vt)(struct Ref *);
    int count;
};

struct Arg3 {
    int f0;
    int f4;
};

void FUN_088695a8(void *self, void *param_2, struct Arg3 *param_3)
{
    struct Ref *r;
    char hash[41];
    char url[256];
    char *base;
    struct Ref *p_ref;

    FUN_08878ab8(param_3->f4, hash);
    base = FUN_08887624();
    FUN_08ba0868(url, 256, DAT_08baf1d8, base, hash);
    FUN_0885d244(self, url);
    r = 0;
    FUN_0885cc04(self, &r, param_2);
    p_ref = r;
    if (p_ref != 0) {
        int c;
        FUN_08ad3088(&DAT_08bef8f4);
        c = --p_ref->count;
        FUN_08ad2810(&DAT_08bef8f4);
        if (c == 0) {
            p_ref->vt[1](p_ref);
        }
    }
}
