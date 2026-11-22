/* LBPMATCH module=EBOOT.BIN start=0x0887c14c end=0x0887c1fc symbol=FUN_0887c14c lang=c++ */
/* Hand-written reconstruction from the disassembly (lane lbp17b, Codex GPT-6): matching idiom: volatile secondary-base field store stays before the loop-entry jump; exact match verified; names and types await human review. */

class Primary
{
public:
    virtual void method0() = 0;
    virtual void method1() = 0;
    virtual int method2() = 0;
    virtual void method3() = 0;
    virtual void method4() = 0;
    virtual void method5() = 0;
    virtual void method6() = 0;
    virtual void method7(int) = 0;
    unsigned int field04;
    unsigned int field08;
};

struct Secondary
{
    unsigned int vtable;
    volatile unsigned int field04;
    unsigned char flag14;
    unsigned char flag15;
};

class Object : public Primary, public Secondary
{
};

extern "C" int FUN_08878e08(Object *);
extern "C" void FUN_0889193c(unsigned int, int);

extern "C" int FUN_0887c14c(void *, Object *param_2, unsigned int param_3)
{
    Secondary *subobject = (Secondary *)((char *)param_2 + 12);
    subobject->field04 = 3;
    while (FUN_08878e08(param_2) == 0) {
        if (*((unsigned char *)param_2 + 0x15) != 0) {
            param_2->method7(0);
            FUN_0889193c(param_3, 1000);
        }
    }
    int result = 6;
    if (param_2->method2() != 0) {
        result = subobject->flag14 != 0 ? 5 : 4;
    }
    return result;
}
