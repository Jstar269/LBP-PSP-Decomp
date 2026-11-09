/* LBPMATCH module=EBOOT.BIN start=0x0892e8e0 end=0x0892e978 symbol=FUN_0892e8e0 lang=c99 */
/* Hand-written reconstruction from the disassembly (lane lbp18c, GPT-6); exact match verified; names and types await human review. */

extern int FUN_0892e5e8(void *, unsigned int, unsigned int, unsigned int,
                        void *, unsigned int, unsigned int, unsigned int);
extern void FUN_08926a30(void *, int);
extern void FUN_0892bb3c(void *);
extern void FUN_08928550(void *);
extern void FUN_0892de08(void *);
extern void FUN_08924590(void *);

void FUN_0892e8e0(void *param_1, unsigned int param_2, unsigned int param_3,
                  unsigned int param_4, void *param_5, unsigned int param_6,
                  unsigned int param_7, unsigned int param_8)
{
    unsigned char *self = (unsigned char *)param_1;
    int result;

    result = FUN_0892e5e8(param_1, param_2, param_3, param_4,
                          param_5, param_6, param_7, param_8);
    if (result == 0 && (self[0x165] & 0x10) == 0) {
        if ((self[0x166] & 0x10) == 0) {
            goto skip_status;
        }
        FUN_08926a30(param_1, 1);
    }
    *(int *)(self + 0xc4) = 1;

skip_status:
    FUN_0892bb3c(param_1);
    FUN_08928550(param_1);
    if (*(int *)(self + 0xc4) == 0) {
        FUN_0892de08(param_1);
        FUN_08924590(param_1);
    }
}
