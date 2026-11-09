/* LBPMATCH module=EBOOT.BIN start=0x08889670 end=0x08889718 symbol=FUN_08889670 lang=c99 */
/* Hand-written reconstruction from the disassembly (lane lbp17b, Codex GPT-6); exact match verified; names and types await human review. */

typedef unsigned short ushort;
extern int FUN_088888ec(ushort **, int);

int FUN_08889670(ushort *destination, int max_chars, ushort *source)
{
    int limit = max_chars - 1;
    int count = 0;
    ushort *out = destination;
    ushort *current = source;
    if (current != 0) {
        while (*current != 0 && count < limit) {
            ushort value = *current;
            if (value == 0x26) {
                value = (ushort)FUN_088888ec(&current, 0);
            } else {
                current++;
            }
            *out++ = value;
            count++;
        }
    }
    destination[count] = 0;
    return count;
}
