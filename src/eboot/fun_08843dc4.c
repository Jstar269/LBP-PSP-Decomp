/* LBPMATCH module=EBOOT.BIN start=0x08843dc4 end=0x08843e64 symbol=FUN_08843dc4 lang=c99 */
/* Hand-written reconstruction from the disassembly (lane lbp16b, deepseek-v4.1); exact match verified; names and types await human review. */
extern char PTR_FUN_08bae388[];
extern char PTR_FUN_08bae3b8[];
extern char DAT_08bedb94[];
extern void FUN_08a9e0c4(void *);
void FUN_08843dc4(char **p)
{
    p[0] = PTR_FUN_08bae388;
    if (p[5] != DAT_08bedb94 && p[5] != 0) FUN_08a9e0c4(p[5]);
    if (p[4] != DAT_08bedb94 && p[4] != 0) FUN_08a9e0c4(p[4]);
    if (p[3] != DAT_08bedb94 && p[3] != 0) FUN_08a9e0c4(p[3]);
    p[0] = PTR_FUN_08bae3b8;
}
