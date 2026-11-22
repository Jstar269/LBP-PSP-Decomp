/* LBPMATCH module=EBOOT.BIN start=0x088706f4 end=0x088707ac symbol=FUN_088706f4 lang=c++ */
/* Hand-written reconstruction from the disassembly (lane oc-lbp-20261121, Antigravity); near match; names and types await human review. */

extern "C" void FUN_08a9d390(void *, unsigned);
extern "C" void FUN_08a9bff8(void *);
extern "C" void **FUN_08870680(void *, unsigned *);

struct Chunk {
    unsigned tag;
    unsigned off;
};

struct Obj {
    unsigned char pad[0x6c];
    int map;
};

struct Tmp {
    long long a;
    int b[4];
};

extern "C" bool FUN_088706f4(Chunk *param_1, char *param_2, Obj *param_3)
{
    switch (param_1->tag) {
    case 0x4d47414d: {
        unsigned key;
        Tmp tmp;
        unsigned *p = (unsigned *)(param_2 + param_1->off);
        key = *p;
        p += 3;
        FUN_08a9d390(&tmp, key);
        FUN_08a9bff8(&tmp);
        *FUN_08870680(&param_3->map, &key) = p;
        return false;
    }
    case 0x20475441:
    case 0x4154454d:
    case 0x53544e45:
        return true;
    default:
        return false;
    }
}
// 36 differing words (47/46): prologue, stack layout (tmp at sp+8), and body all match;
// differs by 1 word at +0x28 in the switch binary-search branch (ours emits `bnezl a0` + `j`, retail emits `lui; beqz a0; ori`).
// near miss from lane oc-lbp-20261121 (unreviewed)
