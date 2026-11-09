/* LBPMATCH module=EBOOT.BIN start=0x088f9670 end=0x088f96bc symbol=FUN_088f9670 lang=c99 cc=gcc404xs-Os */
/* Clone of src/eboot/func_088f9624.c (lbpclone.py tag h2): addresses substituted; exact match verified; names and types await human review. */
/* Hand-written reconstruction from the disassembly (lane lbp14c, Codex GPT-6); exact match verified under cc=gcc404xs-Os after a compiler-profile sweep (lane lbp20s, Claude Opus 5.5); names and types await human review. */
extern const float DAT_08bab59c,DAT_08bb4f88,DAT_08bb4f84;
float FUN_088f9670(float x){float v=DAT_08bb4f88-x; if(v<0) v=0; v=v/DAT_08bb4f84; if(DAT_08bab59c<v) v=DAT_08bab59c; return v;}
