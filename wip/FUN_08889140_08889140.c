/* LBPMATCH module=EBOOT.BIN start=0x08889140 end=0x08889224 symbol=FUN_08889140 lang=c99 */
/* Hand-written reconstruction from the disassembly (lane oc-lbp-20261121, Antigravity); exact match verified; names and types await human review. */

char *FUN_08889140(char *dst, const unsigned short *src, int max_len, int *out_len)
{
    char *p = dst;
    if (max_len == 0) {
        return dst;
    }
    while (*src != 0 && max_len != 1) {
        unsigned short c = *src++;
        if (c < 0x80) {
            *p++ = (char)c;
            max_len--;
        } else if (c < 0x800) {
            max_len -= 2;
            if (max_len == 0) {
                break;
            }
            p[0] = (char)((c >> 6) | 0xc0);
            p[1] = (char)((c & 0x3f) | 0x80);
            p += 2;
        } else {
            if (max_len == 2) {
                break;
            }
            max_len -= 3;
            if (max_len == 0) {
                break;
            }
            p[0] = (char)((c >> 12) | 0xe0);
            p[1] = (char)(((c >> 6) & 0x3f) | 0x80);
            p[2] = (char)((c & 0x3f) | 0x80);
            p += 3;
        }
    }
    *p = 0;
    if (out_len != 0) {
        *out_len = (int)((p + 1) - dst);
    }
    return dst;
}
// near miss from lane oc-lbp-20261121 (unreviewed)
