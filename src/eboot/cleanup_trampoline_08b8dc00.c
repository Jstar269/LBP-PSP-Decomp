/* LBPMATCH module=EBOOT.BIN start=0x08b8dc00 end=0x08b8dc18 symbol=FUN_08b8dc00 lang=c99 */
/* Clone of src/eboot/cleanup_trampoline_08b8dbd0.c (lbpclone.py tag d3): addresses substituted; exact match verified; names and types await human review. */
/* Hand-written reconstruction from the disassembly (lane lbp16d, GPT-6.1); exact match verified; names and types await human review. */
extern void (*DAT_08be4c00)(void);
extern void FUN_08b8dbe8(void (*)(void)) __attribute__((noreturn));

void FUN_08b8dc00(void)
{
    FUN_08b8dbe8(DAT_08be4c00);
}
