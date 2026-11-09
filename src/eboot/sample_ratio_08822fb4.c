/* LBPMATCH module=EBOOT.BIN start=0x08822fb4 end=0x08823048 symbol=FUN_08822fb4 lang=c99 cc=gcc404xs-Os rodata=0x08bac160 */
/* Hand-written reconstruction (external matching lane lbp16a); exact match verified under cc=gcc404xs-Os after a compiler-profile sweep (lane lbp20s, Claude Opus 5.5); names and types await human review. */
typedef unsigned int uint;
typedef struct LocalValues { uint numerator; uint denominator[3]; } LocalValues;
extern void FUN_089f1a94(uint *, uint *);
float FUN_08822fb4(void *param_1)
{
    LocalValues local;
    FUN_089f1a94(&local.numerator, local.denominator);
    return ((float)local.numerator - 794850.0f) / ((float)local.denominator[0] - 794850.0f);
}
