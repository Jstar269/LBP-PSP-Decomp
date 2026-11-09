/* LBPMATCH module=EBOOT.BIN start=0x088369fc end=0x08836ba8 symbol=FUN_088369fc lang=c++ cc=gcc404xs-Os */
/* Hand-written reconstruction (lane lbp19o, Claude Opus 5.5) adapted from src/eboot/map_insert_hint_0880daa4.cpp: libstdc++'s std::_Rb_tree::insert_unique(iterator hint, const value&) (GCC 4.0) for short keys; exact match verified; names and types await human review. */
struct Node { int color; Node *parent; Node *left; Node *right; short key; };
struct Base { int color; Node *parent; Node *left; Node *right; };
struct Tree { int compare; Base header; unsigned count; };
struct Iter { Node *node; Iter(Node *n) : node(n) {} };
struct Pair { Iter first; bool second; Pair(Iter a, bool b) : first(a), second(b) {} };
inline bool lt(const short &a, const short &b) { return a < b; }
extern "C" Node *FUN_08b898a8(Node *n);
extern "C" Node *FUN_08b89838(Node *n);
extern "C" Iter FUN_08836820(Tree *t, Node *x, Node *p, const short &v);
extern "C" Pair FUN_08836914(Tree *t, const short &v);
extern "C" Iter FUN_088369fc(Tree *t, Node *position, const short &v)
{
    if (position == (Node *)&t->header) {
        if (t->count > 0 && lt(t->header.right->key, v))
            return FUN_08836820(t, 0, t->header.right, v);
        else
            return FUN_08836914(t, v).first;
    } else if (lt(v, position->key)) {
        Node *before = position;
        if (position == t->header.left)
            return FUN_08836820(t, t->header.left, t->header.left, v);
        else if (lt((before = FUN_08b898a8(before))->key, v)) {
            if (before->right == 0)
                return FUN_08836820(t, 0, before, v);
            else
                return FUN_08836820(t, position, position, v);
        } else
            return FUN_08836914(t, v).first;
    } else if (lt(position->key, v)) {
        Node *after = position;
        if (position == t->header.right)
            return FUN_08836820(t, 0, t->header.right, v);
        else if (lt(v, (after = FUN_08b89838(after))->key)) {
            if (position->right == 0)
                return FUN_08836820(t, 0, position, v);
            else
                return FUN_08836820(t, after, after, v);
        } else
            return FUN_08836914(t, v).first;
    } else
        return position;
}
