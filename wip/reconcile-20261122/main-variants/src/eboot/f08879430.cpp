/* LBPMATCH module=EBOOT.BIN start=0x08879430 end=0x088794c4 symbol=FUN_08879430 lang=c++ */
/* Hand-written reconstruction from the disassembly (lane lbp17b, Codex GPT-6): matching idiom: volatile secondary-base flag store leaves the call delay slot empty; exact match verified; names and types await human review. */

extern "C" {
extern void FUN_0894a258(void *);
extern void FUN_0888b860(void *);
extern unsigned int DAT_08bb07d0;
extern unsigned int DAT_08bb0804;
}

struct PrimaryBase
{
    unsigned int vtable;
    unsigned int field04;
    unsigned int field08;
};

struct SecondaryBase
{
    unsigned int vtable;
    unsigned int field04;
    volatile unsigned char field08;
};

struct Object : PrimaryBase, SecondaryBase
{
    unsigned char remainder[0xc8 - 0x18];
};

extern "C" void FUN_08879430(Object *param_1, unsigned int param_2,
                              unsigned int param_3, unsigned int param_4)
{
    FUN_0894a258(param_1);
    param_1->PrimaryBase::field04 = 1;
    param_1->PrimaryBase::vtable = (unsigned int)&DAT_08bb07d0;
    SecondaryBase *subobject = param_1;
    subobject->vtable = (unsigned int)&DAT_08bb0804;
    subobject->field04 = 0;
    param_1->PrimaryBase::field08 = 0;
    subobject->field08 = 0;
    FUN_0888b860(&param_1->remainder[0]);
    *(unsigned int *)&param_1->remainder[0xac] = param_4;
    *(unsigned int *)&param_1->remainder[0xa8] = param_3;
    *(unsigned int *)&param_1->remainder[0xa4] = param_2;
    *((unsigned char *)param_1 + 0x15) = 0;
}
