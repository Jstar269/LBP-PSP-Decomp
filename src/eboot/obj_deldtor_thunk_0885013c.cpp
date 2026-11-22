/* LBPMATCH module=EBOOT.BIN start=0x0885013c end=0x0885025c symbol=_ZThn8_N12Obj_08850148D0Ev lang=c++ cc=gcc404xsg-Os */
/* Hand-written reconstruction from the disassembly (lane api-thunk-20261121, Claude Sonnet 5.5): the this-adjusting thunk and the deleting destructor (D0) of Obj_08850148; exact match verified; names and types await human review. */
/* Obj_08850148 derives from PA_08baedc0 (vtable 0x08baedb8) and PB_08baedd8 (vtable 0x08baedd0), both with inline destructors over the root class 0x0894a260; list members (pool-allocated nodes freed under the pool lock) at +20 (node 16 bytes), +28 (node 28 bytes); vtable 0x08baed90 with 3 primary slots. */
struct Pool {
    char pad[32];
    char lock[1];
};
extern "C" Pool *DAT_08be84ac;
extern "C" void FUN_08ad3088(void *);
extern "C" void FUN_08ad2810(void *);
extern "C" void FUN_08adbc24(void *heap, void *p, unsigned size);
struct Node {
    Node *next;
    Node *prev;
};
struct List16 {
    Node *next;
    Node *prev;
    ~List16()
    {
        Node *head = (Node *)this;
        Node *n = head->next;
        while (n != head) {
            Node *nx = n->next;
            Pool *p = DAT_08be84ac;
            FUN_08ad3088(p->lock);
            FUN_08adbc24(p, n, 16);
            FUN_08ad2810(p->lock);
            n = nx;
        }
    }
};
struct List28 {
    Node *next;
    Node *prev;
    ~List28()
    {
        Node *head = (Node *)this;
        Node *n = head->next;
        while (n != head) {
            Node *nx = n->next;
            Pool *p = DAT_08be84ac;
            FUN_08ad3088(p->lock);
            FUN_08adbc24(p, n, 28);
            FUN_08ad2810(p->lock);
            n = nx;
        }
    }
};
struct RootA_0894a260 {
    virtual ~RootA_0894a260();
    virtual void a1();
    char data[4];
};
struct PA_08baedc0 : RootA_0894a260 {
    virtual ~PA_08baedc0() {}
    virtual void a1();
};
struct RootB_0894a260 {
    virtual ~RootB_0894a260();
};
struct PB_08baedd8 : RootB_0894a260 {
    virtual ~PB_08baedd8() {}
    virtual void b1();
    char data[8];
};
struct Obj_08850148 : PA_08baedc0, PB_08baedd8 {
    virtual void a1();
    List16 l16;
    List28 l28;
};
void Obj_08850148::a1()
{
}
