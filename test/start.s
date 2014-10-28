.text
	.align 2
	.global _start

/*http://refspecs.linuxfoundation.org/ELF/ppc64/PPC-elf64abi-1.9.html#RELOC-TYPE*/

_start:
	lis 1, 0x8000
	lis 3, l_toc@highest
	ori 3, 3, l_toc@higher
	sldi 3, 3, 32
	oris 3, 3, l_toc@h
	ori 3, 3, l_toc@l
	ld 2, 0(3)
	bl main

loop:
	b loop

.rodata
	.align 2
l_toc:
	.quad .TOC.@tocbase
