/* LBPMATCH module=EBOOT.BIN start=0x08a94078 end=0x08a940ac symbol=FUN_08a94078 lang=c99 cc=gcc410-Os */
/* Clone of src/eboot/dtor_body_08849d14.c (lbpclone.py tag b2): addresses substituted; exact match verified; names and types await human review. */
/* Hand-written reconstruction from the disassembly; exact match verified; names and types await human review. */
extern char DAT_08bca300[];
extern void FUN_08a0a8ec(void *self);
extern void FUN_08a9e0cc(void *self);
void FUN_08a94078(void **self)
{
    *self = DAT_08bca300;
    FUN_08a0a8ec(self);
    FUN_08a9e0cc(self);
}
