/* LBPMATCH module=EBOOT.BIN start=0x08a41164 end=0x08a41200 symbol=FUN_08a41164 lang=c99  cc=gcc404xs-O2 */
/* Clone of src/eboot/f08a410c8.c (lbpclone.py tag d1): addresses substituted; exact match verified; names and types await human review. */
/* Hand-written reconstruction from the disassembly (lane lbp13a, Codex GPT-6); names and types await human review. */
extern char DAT_08bc4e08; extern void FUN_088050b8(void *), FUN_08a35074(void *);
typedef struct X { void (**vt)(void *); char pad[76]; int field; } X;
void FUN_08a41164(void **p) {
 X *x=(X *)p[100]; p[0]=&DAT_08bc4e08;
 if(x) { x->field=0; x->vt[1](x); }
 if(p[99]) FUN_088050b8(p[99]);
 if(p[98]) FUN_088050b8(p[98]);
 if(p[97]) FUN_088050b8(p[97]);
 if(p[96]) FUN_088050b8(p[96]);
 FUN_08a35074(p);
}
// 2 words differ: vtable pointer held in v0 instead of v1; C++ virtual call also differs.
// near miss from lane lbp13a (unreviewed)
// lane fb-near1 2026-11-02: triaged at the seed's profile (MISMATCH status, 2 differing words); newest residual: 08a410ec 8c820000 lw v0,0(a0) | 8c830000 lw v1,0(a0); seed kept as the best form, further matching not reached in this lane.
