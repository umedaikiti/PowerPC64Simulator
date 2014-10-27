#include <stdio.h>
#include "PowerPC64.h"

int main(int argc, char *argv[])
{
	ppc64_t ppc;
	sim_state_t state = {.ppc = &ppc};
	if(argc < 2){
		printf("usage : %s <filename>\n", argv[0]);
		return 1;
	}
	InitPPC(&ppc, argv[1]);
	PowerPC64MainLoop(&state);
	return 0;
}
