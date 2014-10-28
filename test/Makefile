AS=powerpc-linux-gnu-as

.PHONY:all
all: start.o

start.o: start.s
	$(AS) -a64 $< -o $@
