/* LBPMATCH module=EBOOT.BIN start=0x088b519c end=0x088b5264 symbol=FUN_088b519c lang=c99 */
/* Hand-written reconstruction from the disassembly (lane lbp17c, Codex GPT-6); exact match verified; names and types await human review. */
struct Object1 { void *vtable; };
struct Object2 { void *vtable; };
struct Holder { void *ptr; int *count; };
struct Heap { unsigned char pad[32]; unsigned char lock[1]; };
extern void *FUN_08a9e838(int);
extern void FUN_088c42c4(void *);
extern void FUN_088cc5e0(void *, void *);
extern void FUN_088b2aa4(struct Holder *);
extern void FUN_08804f3c(struct Holder *);
extern void FUN_08ad3088(void *);
extern int *FUN_08adbb84(struct Heap *, int);
extern void FUN_08ad2810(void *);
extern struct Heap *DAT_08be84ac;
extern unsigned char DAT_08bef8f4[];

void FUN_088b519c(void)
{
    void *first = FUN_08a9e838(20);
    FUN_088c42c4(first);
    void *second = FUN_08a9e838(36);
    FUN_088cc5e0(second, first);
    struct Holder holder;
    holder.ptr = second;
    if (second != 0) {
        struct Heap *heap = DAT_08be84ac;
        FUN_08ad3088(heap->lock);
        int *count = FUN_08adbb84(heap, 4);
        FUN_08ad2810(heap->lock);
        holder.count = count;
        FUN_08ad3088(DAT_08bef8f4);
        *count = 1;
        FUN_08ad2810(DAT_08bef8f4);
    } else {
        holder.count = 0;
    }
    FUN_088b2aa4(&holder);
    FUN_08804f3c(&holder);
}



