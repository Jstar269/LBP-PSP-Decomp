/* LBPMATCH module=EBOOT.BIN start=0x088266f0 end=0x088267cc symbol=FUN_088266f0 lang=c++ */
/* Hand-written reconstruction from the disassembly (lane lbp17g, Codex GPT-6.1); exact match verified; names and types await human review. */

class Selector_088266f0 {
public:
    virtual void slot0() = 0;
    virtual void finish() = 0;
    virtual float quantize(float) = 0;
    virtual float get_limit(float) = 0;
    float minimum;
    float maximum;
    float step;
};

extern "C" Selector_088266f0 *FUN_088263a4(int);
extern const float DAT_08bab5c8;

extern "C" void FUN_088266f0(void *, int param_2, float *param_3) {
    Selector_088266f0 *piVar1 = FUN_088263a4(param_2);
    float fVar3 = piVar1->get_limit(*param_3);
    float fVar5 = piVar1->minimum;

    if (!(fVar5 < fVar3)) {
        fVar3 = fVar5;
    }
    float fVar4 = piVar1->maximum;
    if (!(fVar3 < fVar4)) {
        fVar3 = fVar4;
    }
    fVar3 = (float)(int)((fVar3 - fVar5) / piVar1->step + DAT_08bab5c8) * piVar1->step + fVar5;
    if (!(fVar5 < fVar3)) {
        fVar3 = fVar5;
    }
    if (!(fVar3 < fVar4)) {
        fVar3 = fVar4;
    }
    *param_3 = piVar1->quantize(fVar3);
    piVar1->finish();
}

