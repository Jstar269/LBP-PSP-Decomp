/* LBPMATCH module=EBOOT.BIN start=0x08889670 end=0x08889718 symbol=FUN_08889670 lang=c99 */
/* Hand-written reconstruction from the disassembly (lane oc-lbp-20261121, Antigravity); exact match verified; names and types await human review. */

extern int FUN_088888ec(const unsigned short **, unsigned short *);

int FUN_08889670(unsigned short *dst, int max_len, const unsigned short *src)
{
    int limit = max_len - 1;
    int count = 0;
    unsigned short *p = dst;
    const unsigned short *cur = src;

    if (src != 0) {
        while (*cur != 0 && count < limit) {
            unsigned short c = *cur;
            if (c == '&') {
                c = (unsigned short)FUN_088888ec(&cur, 0);
            } else {
                cur++;
            }
            *p++ = c;
            count++;
        }
    }
    dst[count] = 0;
    return count;
}
