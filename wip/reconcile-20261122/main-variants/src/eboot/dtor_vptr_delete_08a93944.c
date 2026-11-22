/* LBPMATCH module=EBOOT.BIN start=0x08a93944 end=0x08a93954 symbol=FUN_08a93944 lang=c99 cc=gcc404xs-Os */
/* Clone of src/eboot/dtor_vptr_delete_088044f8.c (lbpclone.py tag b2): addresses substituted; exact match verified; names and types await human review. */
/* Hand-written reconstruction from the disassembly; exact match verified; names and types await human review. The deleting destructor of the same kind of class: reset the vtable pointer, then free the object (FUN_08a0a8ec, tail call). */
extern char DAT_08bca300[];
extern void FUN_08a0a8ec(void *self);
void FUN_08a93944(void **self)
{
    *self = DAT_08bca300;
    FUN_08a0a8ec(self);
}
