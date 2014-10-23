#include <stdio.h>
#include "PowerPC64.h"

int main(int argc, char *argv[])
{
	ppc64_t ppc;
	if(argc < 2){
		printf("usage : %s <filename>\n", argv[0]);
		return 1;
	}
	InitPPC(&ppc, argv[1]);
	PowerPC64MainLoop(&ppc);
	return 0;
}
