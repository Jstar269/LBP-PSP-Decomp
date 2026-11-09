/* LBPMATCH module=EBOOT.BIN start=0x08b8dbd0 end=0x08b8dbe8 symbol=FUN_08b8dbd0 lang=c99 */
/* Hand-written reconstruction from the disassembly (lane lbp16d, GPT-6.1); exact match verified; names and types await human review. */
extern void (*DAT_08be4bfc)(void);
extern void FUN_08b8db98(void (*)(void)) __attribute__((noreturn));

void FUN_08b8dbd0(void)
{
    FUN_08b8db98(DAT_08be4bfc);
}
