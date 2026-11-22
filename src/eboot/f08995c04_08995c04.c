/* LBPMATCH module=EBOOT.BIN start=0x08995c04 end=0x08995cb4 symbol=FUN_08995c04 lang=c99 */
/* Hand-written reconstruction from the disassembly (lane api-lbp-c-20261121, claude-sonnet-5-5); exact match verified; names and types await human review. */

struct Obj4 { int pad[1]; unsigned char flag; };

extern void FUN_08974360(void);
extern void FUN_089757c0(int a);
extern void FUN_08a50fbc(void);
extern void FUN_08a7e614(void *p, int a, int b);
extern void FUN_0881b25c(void *p);
extern void FUN_0880b148(void *p, int a);
extern int FUN_0884224c(void *p, int a);
extern void FUN_0895cce4(void *p, int a);

extern void *DAT_08bf7304;
extern struct Obj4 *DAT_08be8480;
extern void *DAT_08be8510;
extern void *DAT_08bedb54;
extern void *DAT_08befcac;

void FUN_08995c04(void)
{
  FUN_08974360();
  FUN_089757c0(5);
  FUN_08a50fbc();
  if (DAT_08bf7304 != 0) {
    FUN_08a7e614(DAT_08bf7304, 1, 0);
  }
  if (DAT_08be8480 != 0 && DAT_08be8480->flag != 0) {
    FUN_0881b25c(DAT_08be8510);
    FUN_0880b148(DAT_08be8510, 1);
    if (FUN_0884224c(DAT_08bedb54, 0x20) != 0) {
      FUN_0895cce4(DAT_08befcac, 0x21);
      return;
    }
  }
}
