/* LBPMATCH module=EBOOT.BIN start=0x0889a4c0 end=0x0889a578 symbol=FUN_0889a4c0 lang=c99 */
/* Hand-written reconstruction from the disassembly (lane lbp17c, Codex GPT-6); exact match verified; names and types await human review. */
struct Inner {
    int first;
    int second;
    int third;
    int fourth;
    int fifth;
};
struct Outer {
    unsigned char flag;
    unsigned char padding[3];
    int kind;
    int size;
    struct Inner *inner;
    int reserved;
    int limit;
    void *singleton;
};
struct Local60 { char flag[4]; int value; };
extern void FUN_08aee070(unsigned int *);
extern void *FUN_0889a738(void);
extern int FUN_08aedfe0(struct Outer *);
extern void FUN_08af0738(int *);
extern void FUN_08af09a0(void *);

void FUN_0889a4c0(void *param_1)
{
    struct Local60 local60;
    struct Inner inner;
    struct Outer outer;

    FUN_08aee070((unsigned int *)&outer);
    outer.flag = 1;
    outer.size = 4;
    outer.singleton = FUN_0889a738();
    outer.kind = 2;
    inner.fourth = 0x400;
    outer.inner = &inner;
    inner.fifth = 8;
    inner.second = 8;
    inner.third = 4;
    inner.first = 4;
    outer.limit = 0x100000;
    FUN_08aedfe0(&outer);
    FUN_08af0738((int *)local60.flag);
    local60.flag[0] = 1;
    local60.value = 0x80000;
    FUN_08af09a0(&local60);
    ((unsigned char *)param_1)[16] = 1;
}

