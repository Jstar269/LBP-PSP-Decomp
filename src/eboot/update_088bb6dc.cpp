/* LBPMATCH module=EBOOT.BIN start=0x088bb6dc end=0x088bb890 symbol=FUN_088bb6dc lang=c++ */
/* Hand-written reconstruction from the disassembly (lane lbp21h, Claude Opus 5.5); exact match verified; names and types await human review. */
struct Owned { virtual ~Owned(); };
struct Status { unsigned char pad[80]; bool flag_50; };
struct Item;
struct Object {
    unsigned char pad_00[56];
    Owned *owned;
    unsigned char pad_3c[124];
    void *field_b8;
    unsigned char pad_bc[40];
    bool flag_e4;
    bool flag_e5;
    bool flag_e6;
    bool flag_e7;
    int count;
    void *entries[100];
    unsigned char kinds[100];
    bool flag_2e0;
};
extern "C" int DAT_08beee0c;
extern "C" bool DAT_08bee540;
extern "C" void *FUN_089ecaa8(void);
extern "C" void FUN_0881f424(Owned *, void *);
extern "C" bool FUN_0880b0ec(Owned *);
extern "C" void FUN_08964488(void *);
extern "C" Status *FUN_0895a96c(void);
extern "C" void FUN_0895b95c(Status *, int, int);
extern "C" void FUN_088b64e4(Object *);
extern "C" void FUN_088b6128(Object *);
extern "C" void *FUN_08a9e838(unsigned);
extern "C" void FUN_08a4b630(Item *);
extern "C" void FUN_08a4b78c(Item *, void *, unsigned char, Object *);
extern "C" void *FUN_08a50fbc(void);
extern "C" void FUN_08a537dc(void *, void *);
struct Item {
    unsigned char data[0x220];
    Item() { FUN_08a4b630(this); }
    static void *operator new(unsigned n) { return FUN_08a9e838(n); }
};

extern "C" void FUN_088bb6dc(Object *self)
{
    if (DAT_08beee0c != 0)
        return;
    Owned *owned = self->owned;
    if (owned != 0) {
        FUN_0881f424(owned, FUN_089ecaa8());
        if (FUN_0880b0ec(self->owned)) {
            delete self->owned;
            self->owned = 0;
        }
    }
    FUN_08964488(self->field_b8);
    if (self->flag_e4 || self->flag_e6) {
        if (FUN_0895a96c()->flag_50)
            FUN_0895b95c(FUN_0895a96c(), 0, 0);
    }
    if (self->flag_e7)
        return;
    FUN_088b64e4(self);
    int n;
    if (self->flag_e4) {
        n = self->count;
        self->flag_e4 = false;
        DAT_08bee540 = false;
        self->flag_e7 = true;
        self->flag_2e0 = false;
    } else if (self->flag_e6) {
        n = self->count;
        self->flag_e6 = false;
        self->flag_e7 = true;
        self->flag_2e0 = true;
    } else {
        return;
    }
    if (n > 0) {
        self->count = n - 1;
        Item *item = new Item;
        FUN_08a4b78c(item, self->entries[self->count], self->kinds[self->count], self);
        FUN_08a537dc(FUN_08a50fbc(), self->entries[self->count]);
    } else {
        FUN_088b6128(self);
    }
}

