/* LBPMATCH module=EBOOT.BIN start=0x088bcc10 end=0x088bcce4 symbol=FUN_088bcc10 lang=c99 */
/* Hand-written reconstruction from the disassembly (lane lbp18a, Codex GPT-6); exact match verified; names and types await human review. */
#include "lbp/rd2a_list.h"
#include "lbp/rd2a_object.h"
struct ListHeader { struct ListNode *next; void *prev; int count; };
struct ListPayload { char pad[0xa8]; struct VObject *handler; };
struct ListOwner { char pad_00[0x3c]; struct ListHeader entries; char pad_48[0x18]; char subobject[0x20]; int allocations; };
extern void *DAT_08be84a0;
extern void FUN_088bcb3c(void *);
extern void FUN_088050b8(void *);
extern void FUN_08adbc24(void *, void *, unsigned int);
extern void FUN_08b88fc8(struct ListNode *);
extern void FUN_088bc478(void *, int);
extern void FUN_088bc32c();
extern void FUN_088bb970();
extern void FUN_0894ebcc();

void FUN_088bcc10(struct ListOwner *self)
{
    FUN_088bcb3c(self->subobject);
    if (self->entries.next != (struct ListNode *)&self->entries) {
        struct VObject *handler = ((struct ListPayload *)self->entries.next->data)->handler;
        (*handler->vtable[10])(handler, 0, 0, FUN_088bc32c, self,
                               FUN_088bb970, FUN_0894ebcc);
        struct ListNode *node = self->entries.next;
        FUN_08b88fc8(node);
        if (node->data != 0)
            FUN_088050b8(node->data);
        FUN_08adbc24(DAT_08be84a0, node, 12);
        return FUN_088bc478(self, self->entries.count + 1);
    }
    self->allocations++;
    return FUN_088bc478(self, self->entries.count + 2);
}

