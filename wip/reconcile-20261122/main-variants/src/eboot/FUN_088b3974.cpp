/* LBPMATCH module=EBOOT.BIN start=0x088b3974 end=0x088b3a4c symbol=FUN_088b3974 lang=c++ */
/* Hand-written reconstruction from the disassembly (lane lbp18a, Codex GPT-6); exact match verified; names and types await human review. */
struct Item { char pad[17]; unsigned char handled; };
struct Mutex { int word; };
struct Owner { void *vtable; void *list; char pad_08[12]; Mutex mutex; };
struct Resource {
    virtual void release0();
    virtual void release1();
    virtual void release2();
    int references;
    char pad_08[12];
    int kind;
};
extern "C" int DAT_08bef8f4;
extern "C" void FUN_08ad3088(void *);
extern "C" void FUN_08ad2810(void *);
extern "C" void FUN_08804f34() __attribute__((weak));
struct Handle {
    Resource *p;
    ~Handle() {
        Resource *resource = p;
        if (resource != 0) {
            FUN_08ad3088(&DAT_08bef8f4);
            int count = --resource->references;
            FUN_08ad2810(&DAT_08bef8f4);
            if (count == 0)
                resource->release2();
        }
    }
};
extern "C" Handle FUN_088b38a0();
extern "C" void FUN_088b3728(void *, const Handle *);

extern "C" int FUN_088b3974(Owner *owner, Item **current)
{
    int success = 0;
    Item *item = *current;
    if (item != 0) {
        if (FUN_08804f34 != 0) {
            if (item->handled == 0) {
                item->handled = 1;
                {
                    Handle handle = FUN_088b38a0();
                    handle.p->kind = 2;
                    FUN_08ad3088(&owner->mutex);
                    FUN_088b3728(&owner->list, &handle);
                    FUN_08ad2810(&owner->mutex);
                }
                success = 1;
            }
        }
    }
    return success;
}



