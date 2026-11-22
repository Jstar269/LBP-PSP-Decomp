/* LBPMATCH module=EBOOT.BIN start=0x08863de8 end=0x08863ebc symbol=FUN_08863de8 lang=c99 */
/* Hand-written reconstruction from the disassembly (lane lbp17h, GPT-6.1 Terra); exact match verified; names and types await human review. */
typedef struct Ref { void (**vtable)(struct Ref *); int refs; } Ref;
typedef struct { unsigned int reserved; Ref **entries; int count; int next; int allocations; } Pool;
extern Pool DAT_08bedd68;
extern char DAT_08bef8f4;
extern void FUN_08ad3088(void *);
extern void FUN_08ad2810(void *);
extern Ref *FUN_08adb810(unsigned int, unsigned int, unsigned int, unsigned int);
extern void FUN_088638fc(Ref *);
Ref **FUN_08863de8(Ref **out) {
    Ref *value;
    void *loaded;
    FUN_08ad3088(&DAT_08bef8f4);
    FUN_08ad3088(&DAT_08bef8f4);
    if (DAT_08bedd68.next < DAT_08bedd68.count) {
        loaded = DAT_08bedd68.entries[DAT_08bedd68.next];
        ++DAT_08bedd68.next;
        value = (Ref *)loaded;
        if (loaded != 0) {
            FUN_088638fc((Ref *)loaded);
        }
    } else {
        ++DAT_08bedd68.allocations;
        value = FUN_08adb810(0xe4, 0x10, 0, 0);
        FUN_088638fc(value);
    }
    FUN_08ad2810(&DAT_08bef8f4);
    *out = value;
    FUN_08ad2810(&DAT_08bef8f4);
    return out;
}

