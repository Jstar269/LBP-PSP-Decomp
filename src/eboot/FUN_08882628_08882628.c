/* LBPMATCH module=EBOOT.BIN start=0x08882628 end=0x088826e4 symbol=FUN_08882628 lang=c99 */
/* Hand-written reconstruction from the disassembly (lane oc-lbp-20261121, Antigravity); exact match verified; names and types await human review. */

extern unsigned char DAT_08bee04c;
extern unsigned char DAT_08bee04d;
extern unsigned char DAT_08bee050[2];
extern int DAT_08bf8610;

struct Mgr {
    unsigned char pad[80];
    unsigned char b50;
};

extern struct Mgr *FUN_0895a96c(void);
extern void FUN_08a9e39c(void);
extern void FUN_08a9de28(void);

void FUN_08882628(void)
{
    if (DAT_08bee04c != 0) {
        if (DAT_08bee050[0] == 0 && FUN_0895a96c()->b50 == 0) {
            DAT_08bee050[0] = 1;
        }
    } else {
        DAT_08bee050[0] = 0;
    }

    if (DAT_08bee04d != 0) {
        if (DAT_08bee050[1] == 0 && DAT_08bf8610 != 0) {
            FUN_08a9e39c();
            DAT_08bee050[1] = 1;
        }
    } else if (DAT_08bee050[1] != 0) {
        DAT_08bee050[1] = 0;
        FUN_08a9de28();
    }
}
