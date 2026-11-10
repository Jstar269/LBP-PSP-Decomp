/* LBPMATCH module=EBOOT.BIN start=0x088c1634 end=0x088c17e8 symbol=FUN_088c1634 lang=c++ */
/* Hand-written reconstruction from the disassembly (lane lbp21h, Claude Opus 5.5); exact match verified; names and types await human review. */
struct Mutex { int a; };
extern "C" Mutex DAT_08bef8f4;
extern "C" void FUN_08ad3088(void *);
extern "C" void FUN_08ad2810(void *);
struct StrRef;
extern "C" int FUN_08a9b8a4(StrRef *, const char *);
struct StrRef {
    const char *str; int len;
    StrRef(const char *s) { int n = FUN_08a9b8a4(this, s); str = s; len = n; }
};
struct Path;
extern "C" void FUN_08a9c164(Path *, const char *);
extern "C" void FUN_08a9bff8(Path *);
extern "C" void FUN_08aaa9a0(Path *);
extern "C" void FUN_08a9c860(Path *, const StrRef &, const StrRef &);
extern "C" void FUN_08a9c5b0(Path *, const Path &);
struct Path {
    long long data[3];
    Path(const char *s) { FUN_08a9c164(this, s); }
    ~Path() { FUN_08a9bff8(this); }
};
struct Target { unsigned char pad[26]; unsigned char busy; };
typedef void (*Callback)(void);
extern "C" void FUN_088c0b70(void);
extern "C" void FUN_088c0ae8(void);
struct Job {
    virtual void vf0();
    virtual ~Job();
    int refs;
    int field_08, field_0c;
    Path path;
    int field_28;
    Callback on_done;
    Target *done_arg;
    Callback on_fail;
    Target *fail_arg;
    int priority;
};
struct JobRef {
    Job *p;
    ~JobRef() {
        Job *q = p;
        if (q) {
            FUN_08ad3088(&DAT_08bef8f4);
            int n = --q->refs;
            FUN_08ad2810(&DAT_08bef8f4);
            if (n == 0)
                delete q;
        }
    }
    Job *operator->() const { return p; }
};
extern "C" JobRef FUN_088c1358(void);
extern "C" void FUN_0893862c(const JobRef &);
struct Settings { unsigned char pad[0x206c]; float level; };
extern "C" Settings *FUN_08a10f1c(void);
struct Chunk { unsigned tag; int offset; };
struct WorldData { float level; char name[1]; };
extern "C" char DAT_08bb39e0[];
extern "C" char DAT_08bb380c[];

extern "C" bool FUN_088c1634(Chunk *chunk, char *base, Target *target)
{
    unsigned tag = chunk->tag;
    if (tag == 0x444c5257) {
        WorldData *d = (WorldData *)(base + chunk->offset);
        FUN_08a10f1c()->level = d->level;
        if (d->name[0] != 0) {
            Path path(d->name);
            FUN_08aaa9a0(&path);
            FUN_08a9c860(&path, StrRef(DAT_08bb39e0), StrRef(DAT_08bb380c));
            JobRef job = FUN_088c1358();
            FUN_08a9c5b0(&job->path, path);
            job->priority = 20;
            job->on_done = FUN_088c0b70;
            job->on_fail = FUN_088c0ae8;
            target->busy = 0;
            job->fail_arg = target;
            job->done_arg = target;
            FUN_0893862c(job);
        }
        return false;
    }
    return tag == 0x20475441 || tag == 0x53544e45;
}
