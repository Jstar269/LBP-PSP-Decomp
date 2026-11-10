/* LBPMATCH module=EBOOT.BIN start=0x088d536c end=0x088d5524 symbol=FUN_088d536c lang=c++ */
/* Hand-written reconstruction from the disassembly (lane lbp21h, Claude Opus 5.5); matching idiom: the last load (kind 4) is written out in place instead of through the load() helper, which gives retail's temporary stack slot order; exact match verified; names and types await human review. */
struct Path;
struct View { int data; int size; View(const Path &p); };
extern "C" void FUN_08a9c014(Path *);
extern "C" void FUN_08a9bff8(Path *);
struct Path {
    int data; int size; int field_08, field_0c, field_10, field_14;
    Path() { FUN_08a9c014(this); }
    ~Path() { FUN_08a9bff8(this); }
};
inline View::View(const Path &p) : data(p.data), size(p.size) {}
struct Object;
extern "C" void FUN_088d4ae0(Object *, Path &, int);
extern "C" void FUN_088ebab8(void *, const View &, bool);
struct Table;
extern "C" void FUN_088cd5d4(Table *, Object *, bool);
extern "C" void FUN_088d2548(Table *, Object *);
struct Manager { unsigned char pad[0x1c4]; Table *table_1c4; Table *table_1c8; };
extern "C" Manager *DAT_08beed14;
extern "C" void *DAT_08beee0c;
struct Object {
    unsigned flag_bits : 4;
    unsigned dirty : 1;
};
static inline void load(Object *self, int kind)
{
    Path p;
    FUN_088d4ae0(self, p, kind);
    FUN_088ebab8(DAT_08beee0c, p, false);
}
extern "C" void FUN_088d536c(Object *self)
{
    load(self, 3);
    load(self, 0);
    load(self, 1);
    load(self, 2);
    { Path p; FUN_088d4ae0(self, p, 4); FUN_088ebab8(DAT_08beee0c, p, false); }
    if (self->dirty) {
        self->dirty = 0;
        FUN_088cd5d4(DAT_08beed14->table_1c4, self, false);
    }
    FUN_088d2548(DAT_08beed14->table_1c8, self);
}
