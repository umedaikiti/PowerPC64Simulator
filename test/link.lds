OUTPUT_FORMAT("elf64-powerpc");
OUTPUT_ARCH(powerpc:common64);
ENTRY(_start);
SECTIONS{
	. = 0x10000000;
	.text : { *(.text .text.*) }
	. = ALIGN(0x200);
	.rodata : { *(.rodata) }
	. = ALIGN(0x200);
	.data : { *(.data) }
	. = ALIGN(0x200);
	.bss : { *(.bss) }
	. = ALIGN(0x200);
}
