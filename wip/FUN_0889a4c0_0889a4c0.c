/* LBPMATCH module=EBOOT.BIN start=0x0889a4c0 end=0x0889a578 symbol=FUN_0889a4c0 lang=c99 */
/* Hand-written reconstruction from the disassembly (lane oc-lbp-20261121, Antigravity); exact match verified; names and types await human review. */

extern void FUN_08aee070(void *);
extern void *FUN_0889a738(void);
extern void FUN_08aedfe0(void *);
extern void FUN_08af0738(void *);
extern void FUN_08af09a0(void *);

struct S1 {
    unsigned char b0;
    unsigned char pad[3];
    int f4;
};

struct S2 {
    int f0;
    int f4;
    int f8;
    int f0c;
    int f10;
};

struct S3 {
    unsigned char b0;
    unsigned char pad[3];
    int f4;
    int f8;
    struct S2 *f0c;
    int f10;
    int f14;
    void *f18;
};

struct Obj {
    unsigned char pad[16];
    unsigned char b10;
};

void FUN_0889a4c0(struct Obj *self)
{
    struct S1 s1;
    struct S2 s2;
    struct S3 s3;

    FUN_08aee070(&s3);
    s3.b0 = 1;
    s3.f8 = 4;
    s3.f18 = FUN_0889a738();
    s3.f4 = 2;
    s2.f0 = 4;
    s2.f4 = 8;
    s2.f8 = 4;
    s2.f0c = 0x400;
    s2.f10 = 8;
    s3.f0c = &s2;
    s3.f14 = 0x100000;
    FUN_08aedfe0(&s3);
    FUN_08af0738(&s1);
    s1.b0 = 1;
    s1.f4 = 0x80000;
    FUN_08af09a0(&s1);
    self->b10 = 1;
}
// near miss from lane oc-lbp-20261121 (unreviewed)
