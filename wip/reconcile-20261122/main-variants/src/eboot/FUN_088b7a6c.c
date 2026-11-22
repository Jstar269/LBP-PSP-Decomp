/* LBPMATCH module=EBOOT.BIN start=0x088b7a6c end=0x088b7b18 symbol=FUN_088b7a6c lang=c99 */
/* Hand-written reconstruction from the disassembly (lane lbp17c, Codex GPT-6); exact match verified; names and types await human review. */
struct FirstRef { unsigned char pad_00[8]; int references; unsigned char pad_0c[8]; unsigned char lock[1]; };
extern void FUN_08ad3088(void *);
extern void FUN_08ad2810(void *);
extern unsigned char DAT_08bef8f4[];

void FUN_088b7a6c(int *destination, int *source)
{
    struct FirstRef *first = (struct FirstRef *)source[0];
    destination[0] = (int)first;
    if (first != 0) {
        FUN_08ad3088(first->lock);
        first->references++;
        FUN_08ad2810(first->lock);
    }
    int *second = (int *)source[1];
    destination[1] = (int)second;
    if (second != 0) {
        FUN_08ad3088(DAT_08bef8f4);
        second[1]++;
        FUN_08ad2810(DAT_08bef8f4);
    }
    destination[2] = source[2];
}

