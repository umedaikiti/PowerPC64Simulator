#ifndef _REGISTERS_H_
#define _REGISTERS_H_

typedef struct {
	unsigned long long GPR[32];
	double FPR[32];
	union {
		unsigned long long raw;
		struct {
			unsigned int cr7 : 4;
			unsigned int cr6 : 4;
			unsigned int cr5 : 4;
			unsigned int cr4 : 4;
			unsigned int cr3 : 4;
			unsigned int cr2 : 4;
			unsigned int cr1 : 4;
			unsigned int cr0 : 4;
			unsigned int blank;
		} field;
	} CR;
	unsigned long long LR;
	unsigned long long CTR;
	union {
		unsigned long long raw;
		struct {
			unsigned int bc : 7;
			unsigned int reserved2 : 22;
			unsigned int ca : 1;
			unsigned int ov : 1;
			unsigned int so : 1;
			unsigned int reserved1;
		} field;
	} XER;
} regs_t;

#endif
