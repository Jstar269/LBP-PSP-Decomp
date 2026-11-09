/* LBPMATCH module=EBOOT.BIN start=0x088237d4 end=0x08823894 symbol=FUN_088237d4 lang=c99 rodata=0x08bac148 */
/* Hand-written reconstruction from the disassembly (lane lbp16a, GPT-6); exact match verified; names and types await human review. */
typedef unsigned int uint;
typedef float (*SampleFn)(void *);
typedef struct FunctionTable { SampleFn entries[6]; } FunctionTable;
typedef struct State { float current; float previous; uint index; } State;
extern float FUN_08822fb4(void *);
extern float FUN_08823048(void *);
extern float FUN_088231a0(void *);
extern float FUN_088231d8(void *);
extern float FUN_088230a0(void *);
extern float FUN_08823170(void *);
static const FunctionTable sample_functions = {{
    FUN_08822fb4, FUN_08823048, FUN_088231a0,
    FUN_088231d8, FUN_088230a0, FUN_08823170
}};
void FUN_088237d4(State *state)
{
    uint remaining = 6;
    do {
        FunctionTable local = sample_functions;
        float sample = local.entries[state->index](state);
        if (!(state->current < sample))
            sample = state->current;
        state->current = sample;
        state->index++;
        if (state->index == 6) {
            state->previous = sample;
            state->index = 0;
            state->current = 0.0f;
        }
        remaining--;
    } while (remaining != 0);
}
