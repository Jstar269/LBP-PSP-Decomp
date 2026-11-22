/* LBPMATCH module=EBOOT.BIN start=0x08970c00 end=0x08970cac symbol=FUN_08970c00 lang=c99 rodata=0x08bbd07c */
/* Hand-written reconstruction from the disassembly (lane api-lbp-b-20261121, claude-sonnet-5-5); exact match verified; names and types await human review. */
extern void FUN_08baa964(int a, int *p);

int FUN_08970c00(int unused, int kind)
{
  int r = 0;
  switch (kind) {
  case 1:
    r = 4;
    break;
  case 2:
  case 15:
    r = 1;
    break;
  case 3:
    r = 3;
    break;
  case 5:
    r = 2;
    break;
  case 6:
    r = 5;
    break;
  case 7:
    r = 0;
    break;
  case 8:
    r = 9;
    break;
  case 9:
    r = 6;
    break;
  case 12:
    r = 7;
    break;
  case 13:
    r = 8;
    break;
  case 16:
    r = 10;
    break;
  default:
    FUN_08baa964(8, &r);
    break;
  }
  return r;
}
