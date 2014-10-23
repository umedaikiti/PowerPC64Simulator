OBJS = PowerPC64.o main.o Instructions.o Memory.o Mnemonic.o
CFLAGS = -g -Wall -Wextra -Wno-unused-parameter

.PHONY: all
all: depend powerpc64sim

.PHONY: depend
depend: $(OBJS:.o=.d)

.SUFFIXES: .d
.c.d:
	gcc -MM $< | sed 's/\($*\).o[ :]*/\1.o $@ : /g' > $@

-include $(OBJS:.o=.d)

powerpc64sim: $(OBJS)
	gcc -o $@ $^ $(CFLAGS)

.c.o:
	gcc $(CFLAGS) -c $<

.PHONY:clean
clean:
	rm -rf powerpc64sim $(OBJS) $(OBJS:.o=.d)

