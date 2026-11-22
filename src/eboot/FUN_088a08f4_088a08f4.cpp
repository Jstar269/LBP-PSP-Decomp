/* LBPMATCH module=EBOOT.BIN start=0x088a08f4 end=0x088a09a0 symbol=FUN_088a08f4 lang=c++ */
/* Hand-written reconstruction from the disassembly (lane oc-lbp-20261121, Antigravity); exact match verified; names and types await human review. */

struct Item {
    int f0;
    int f4;
};

struct Vec;
extern "C" void FUN_088a0794(Vec *, Item **, Item **);

struct Vec {
    Item **begin;
    Item **end;
    Item **cap;
    inline void push_back(Item *x)
    {
        Item **e = end;
        if (e != cap) {
            if (e != 0) {
                *e = x;
            }
            end = e + 1;
        } else {
            FUN_088a0794(this, e, &x);
        }
    }
};

extern "C" Vec DAT_08bee390;
extern "C" int DAT_08bee38c;
extern "C" char DAT_08bb1fd0[];
extern "C" char DAT_08bb1ff4[];

extern "C" void FUN_088a0b98(void);
extern "C" void FUN_088a05cc(void);
extern "C" int FUN_08b2d954(int *, void (*)(void), void (*)(void), int);
extern "C" void FUN_0889a8d0(char *, int);

extern "C" void FUN_088a08f4(Item *param_1)
{
    DAT_08bee390.push_back(param_1);
    int r = FUN_08b2d954(&param_1->f4, FUN_088a0b98, FUN_088a05cc, 0);
    FUN_0889a8d0(DAT_08bb1fd0, r);
    FUN_0889a8d0(DAT_08bb1ff4, param_1->f4);
    DAT_08bee38c++;
}
