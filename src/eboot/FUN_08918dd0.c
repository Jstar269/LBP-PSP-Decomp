/* LBPMATCH module=EBOOT.BIN start=0x08918dd0 end=0x08918e84 symbol=FUN_08918dd0 lang=c99 */
/* Hand-written reconstruction from the disassembly (lane lbp18b, GPT-6); exact match verified; names and types await human review. */
struct State { void *buffer; int field4; int field8; unsigned char active; unsigned char pad[3]; };
extern struct State *DAT_08bef4f4;
extern void *FUN_08a9e838(unsigned int);
extern void FUN_08b9eb88(void *, int, unsigned int);
extern void FUN_08b15664(void);
extern void FUN_08b15cd8(int);
extern void FUN_08baa11c(int);
extern void FUN_08b15718(void);
void FUN_08918dd0(void)
{
    if (DAT_08bef4f4 == 0) {
        struct State *state = FUN_08a9e838(0x10);
        state->field4 = 0;
        state->field8 = 0;
        state->active = 0;
        state->pad[0] = 0;
        state->pad[1] = 0;
        state->pad[2] = 0;
        void *buffer = FUN_08a9e838(0x34);
        FUN_08b9eb88(buffer, 0, 0x34);
        state->buffer = buffer;
        DAT_08bef4f4 = state;
    }
    struct State *state = DAT_08bef4f4;
    if (state->active != 0) {
        FUN_08b15664();
        FUN_08b15cd8(state->field8);
        state->field8 = 0;
        state->active = 0;
    }
    FUN_08baa11c(15000);
    FUN_08b15718();
}
