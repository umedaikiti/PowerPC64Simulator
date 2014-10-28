TARGETS=fib_rec.bin
CC=powerpc-linux-gnu-gcc
LD=powerpc-linux-gnu-ld
OBJCOPY=powerpc-linux-gnu-objcopy
CFLAGS=-O2 -fno-inline-small-functions -m64
LDFLAGS=-m elf64ppc
RM=rm

.PHONY:all
all: $(TARGETS)

.PHONY: clean
clean:
	$(RM) -rf $(TARGETS) $(TARGETS:%.bin=%.o) $(TARGETS:%.bin=%)

$(TARGETS): %.bin: %
$(TARGETS:%.bin=%): %: %.o

#暗黙のルールのキャンセル
%: %.c

%.bin: %
	$(OBJCOPY) -j .text -j .data -j .rodata -O binary $< $@

%: %.o ../link.lds ../start.o
	$(LD) $(LDFLAGS) -T ../link.lds ../start.o $< -o $@

%.o: %.c
	$(CC) $(CFLAGS) -c $<

