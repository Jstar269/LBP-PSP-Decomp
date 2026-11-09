/* LBPMATCH module=EBOOT.BIN start=0x0889ebc4 end=0x0889ec90 symbol=FUN_0889ebc4 lang=c99 */
/* Hand-written reconstruction from the disassembly (lane lbp18a, Codex GPT-6): matching idiom: volatile initialization stores preserve retail store order; exact match verified; names and types await human review. */
typedef struct Vector { char *begin; char *end; char *capacity; } Vector;
typedef struct Buffer { char *data; char *size; char *capacity; } Buffer;
typedef struct Object {
    void *vtable;
    unsigned int f4, f8, f12, f16;
    unsigned char bytes20[4];
    unsigned int f24, f28, f32;
    Vector vector36;
    Vector vector48;
    Buffer buffer60;
    unsigned int f72, f76, f80;
    unsigned char byte84;
} Object;
extern char *FUN_0889de48(Vector *, char *, char *);
extern char *FUN_0889d4d4(Vector *, char *, char *);
extern void *FUN_0889d3a4(Buffer *, char *, char *);

void *FUN_0889ebc4(Object *raw)
{
    volatile Object *object = raw;
    Vector *vector36 = &raw->vector36;
    Vector *vector48 = &raw->vector48;
    Buffer *buffer60 = &raw->buffer60;
    volatile Vector *vector48_init = (volatile Vector *)&raw->vector48;
    volatile Buffer *buffer60_init = (volatile Buffer *)&raw->buffer60;

    object->vtable = 0;
    object->f4 = 0;
    object->f8 = 0;
    object->f12 = 0;
    object->f16 = 0;
    object->bytes20[0] = 0;
    object->bytes20[1] = 0;
    object->bytes20[2] = 0;
    object->bytes20[3] = 0;
    object->f24 = 0;
    object->f28 = 0;
    object->f32 = 0;
    object->vector36.begin = 0;
    vector48_init->begin = 0;
    vector48_init->end = 0;
    vector48_init->capacity = 0;
    buffer60_init->data = 0;
    buffer60_init->size = 0;
    buffer60_init->capacity = 0;
    object->f72 = 0;
    object->f76 = 0;
    object->f80 = 0;
    object->byte84 = 0;
    vector36->end = 0;
    vector36->capacity = 0;
    FUN_0889de48(vector36, 0, 0);
    FUN_0889d4d4(vector48, vector48->begin, vector48->end);
    return FUN_0889d3a4(buffer60, buffer60->data, buffer60->size);
}

