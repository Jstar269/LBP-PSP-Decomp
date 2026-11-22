/* LBPMATCH module=EBOOT.BIN start=0x08889048 end=0x08889140 symbol=FUN_08889048 lang=c99 */
/* Hand-written reconstruction from the disassembly (lane oc-lbp-20261121, Antigravity); exact match verified; names and types await human review. */

unsigned short *FUN_08889048(unsigned short *dst, const unsigned char *src, int max_len, int *out_len)
{
    unsigned short *p = dst;
    int i = 0;

    if (max_len == 0) {
        return dst;
    }
    while (1) {
        unsigned char c = *src;
        if (c == 0) {
            *p = 0;
            break;
        }
        if (i == max_len - 1) {
            *p = 0;
            break;
        }
        if ((signed char)c >= 0) {
            *p++ = c;
            src++;
        } else if (c < 0xe0) {
            *p++ = (unsigned short)(((c & 0x1f) << 6) | (src[1] & 0x3f));
            src = (src[1] != 0) ? (src + 2) : (src + 1);
        } else {
            *p++ = (unsigned short)(((c & 0xf) << 12) | ((src[1] & 0x3f) << 6) | (src[2] & 0x3f));
            if (src[1] == 0) {
                src += 1;
            } else if (src[2] == 0) {
                src += 2;
            } else {
                src += 3;
            }
        }
        i++;
    }
    if (out_len != 0) {
        *out_len = (int)((p - dst) + 1);
    }
    return dst;
}
// near miss from lane oc-lbp-20261121 (unreviewed)
