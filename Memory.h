#ifndef _MEMORY_H_
#define _MEMORY_H_
/*
 * メモリ管理の方法を変更してもプログラムの他の部分を変更する必要が無いように気をつける
 */

#define MAX_PAGES 128
#define PAGE_SIZE (1 << 20)
#define PAGE_MASK (~((unsigned long long)PAGE_SIZE - 1))
/*
 * 1MBごとのページに分割し、存在しないページに書き込むとmallocする
 * 存在しないページを読み込むと0
 * ページが存在するか否かはpage_addressをリニアサーチして探す
 * page_count <= MAX_PAGESに注意
 */
typedef struct _memory_t {
	unsigned char *pages[MAX_PAGES];
	unsigned long long page_address[MAX_PAGES];
	int page_count;
} memory_t;

int ReadMemory8(memory_t *memory, unsigned long long address, unsigned long long *data);
int ReadMemory4(memory_t *memory, unsigned long long address, unsigned int *data);
int ReadMemory2(memory_t *memory, unsigned long long address, unsigned short *data);
int ReadMemory1(memory_t *memory, unsigned long long address, unsigned char *data);

int WriteMemory8(memory_t *memory, unsigned long long address, unsigned long long data);
int WriteMemory4(memory_t *memory, unsigned long long address, unsigned int data);
int WriteMemory2(memory_t *memory, unsigned long long address, unsigned short data);
int WriteMemory1(memory_t *memory, unsigned long long address, unsigned char data);


int WriteMemory(memory_t *memory, unsigned long long address, const unsigned char *data, int n);

#endif //_MEMORY_H_

