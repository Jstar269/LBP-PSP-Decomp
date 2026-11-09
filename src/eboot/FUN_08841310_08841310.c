/* LBPMATCH module=EBOOT.BIN start=0x08841310 end=0x088413dc symbol=FUN_08841310 lang=c99 */
/* Hand-written reconstruction from the disassembly (lane lbp17g, Codex GPT-6.1); exact match verified; names and types await human review. */

typedef unsigned int uint;
typedef unsigned char byte;
typedef struct String_08841310 {
    int unknown;
    int length;
    char *data;
} String_08841310;
typedef struct NameInfo_08841310 {
    char *name;
    int value;
} NameInfo_08841310;
typedef struct Entry_08841310 {
    uint self;
    uint input;
    uint spare_08;
    uint copied;
    uint spare_10;
    uint cleared;
} Entry_08841310;

extern char *DAT_08bdb11c;
extern void *FUN_08a9e838(int);
extern void FUN_089085d8(void *, String_08841310 *, uint *);
extern long long FUN_08a9b8a4(char **, uint *);
extern void FUN_08908bb4(void *, NameInfo_08841310 *);
extern void FUN_08908d54(void *, uint *);
extern void FUN_08841000(uint *);

_Bool FUN_08841310(uint *param_1, String_08841310 *param_2) {
    Entry_08841310 *entry = (Entry_08841310 *)(param_1 + 1);
    if (param_1[0] != 0) {
        return 0;
    }
    void *object = FUN_08a9e838(0x80);
    FUN_089085d8(object, param_2, param_1 + 8);
    char *name = DAT_08bdb11c;
    param_1[7] = (uint)object;
    NameInfo_08841310 local;
    long long pair = FUN_08a9b8a4(&local.name, (uint *)name);
    local.value = (int)pair;
    local.name = name;
    FUN_08908bb4(object, &local);
    param_1[0] = 1;
    FUN_08908d54((void *)param_1[7], param_1 + 2);
    entry->self = (uint)param_1;
    entry->copied = entry->input;
    entry->cleared = 0;
    FUN_08841000((uint *)entry);
    return 1;
}


