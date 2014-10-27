#ifndef _POWERPC64_H_
#define _POWERPC64_H_

#include "Registers.h"
#include "Memory.h"

/*
 * PowerPCの状態はすべてこの構造体に
 */
typedef struct _ppc64_t {
	regs_t regs;
	memory_t memory;
	unsigned long long nia, cia;
} ppc64_t;

static inline int setCR(ppc64_t *ppc, int n, unsigned int cr)
{
	unsigned int shift = 4*(7-n);
	cr &= 0xf;
	ppc->regs.CR.raw &= ~(0xFull << shift);
	ppc->regs.CR.raw |= cr << shift;
	return 0;
}

//0で初期化する
typedef struct {
	ppc64_t *ppc;
	unsigned long long step_count;
	int state;
	unsigned long long breakpoint_addr;
	int breakpoint_enable;
} sim_state_t;
#define STATE_NORMAL 0
#define STATE_CONTINUE 1

int InitPPC(ppc64_t *ppc, const char *filename);

int PowerPC64MainLoop(sim_state_t *state);

#endif
