/* LBPMATCH module=EBOOT.BIN start=0x088bc478 end=0x088bc630 symbol=FUN_088bc478 lang=c++ */
/* Hand-written reconstruction from the disassembly (lane lbp21h, Claude Opus 5.5); exact match verified; names and types await human review. */
struct Mutex { int a; };
extern "C" Mutex DAT_08bef8f4;
extern "C" char *DAT_08be84ac;
extern "C" void *DAT_08be85fc;
extern "C" void FUN_08ad3088(void *);
extern "C" void FUN_08ad2810(void *);
extern "C" void FUN_08adbc24(void *heap, void *p, unsigned size);
extern "C" void FUN_08adb8a4(void *p);
struct Holder {
    void *ptr; int *count;
    ~Holder() {
        int *c = count;
        if (c) {
            FUN_08ad3088(&DAT_08bef8f4);
            int n = --*c;
            FUN_08ad2810(&DAT_08bef8f4);
            if (n == 0) {
                if (ptr)
                    FUN_08adb8a4(ptr);
                char *heap = DAT_08be84ac;
                int *cc = count;
                FUN_08ad3088(heap + 32);
                FUN_08adbc24(heap, cc, 4);
                FUN_08ad2810(heap + 32);
            }
        }
    }
};
struct String;
extern "C" void FUN_0897633c(String *, const short *);
struct String {
    int field_00; int field_04; Holder buf;
    String(const short *s) { FUN_0897633c(this, s); }
};
struct Widget;
extern "C" const short *FUN_08970dbc(void *table, int id);
extern "C" void FUN_089d1dfc(Widget *, const String &, bool);
extern "C" void FUN_089d299c(Widget *, int, int, bool);
struct Object {
    unsigned char pad_00[68];
    int mode;
    Widget *label_48;
    Widget *label_4c;
};
extern "C" void FUN_088bc478(Object *self, int mode)
{
    if (self->mode == 0 && mode != 12) {
        FUN_089d1dfc(self->label_48, String(FUN_08970dbc(DAT_08be85fc, 225)), false);
        FUN_089d1dfc(self->label_4c, String(FUN_08970dbc(DAT_08be85fc, 453)), false);
        FUN_089d299c(self->label_4c, 360, 0, false);
    }
    self->mode = mode;
}
