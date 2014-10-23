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

struct _ppc64_t;
typedef struct _ppc64_t ppc64_t;

int ReadMemory8(ppc64_t *ppc, unsigned long long address, unsigned long long *data);
int ReadMemory4(ppc64_t *ppc, unsigned long long address, unsigned int *data);
int ReadMemory2(ppc64_t *ppc, unsigned long long address, unsigned short *data);
int ReadMemory1(ppc64_t *ppc, unsigned long long address, unsigned char *data);

int WriteMemory8(ppc64_t *ppc, unsigned long long address, unsigned long long data);
int WriteMemory4(ppc64_t *ppc, unsigned long long address, unsigned int data);
int WriteMemory2(ppc64_t *ppc, unsigned long long address, unsigned short data);
int WriteMemory1(ppc64_t *ppc, unsigned long long address, unsigned char data);


int WriteMemory(ppc64_t *ppc, unsigned long long address, const unsigned char *data, int n);

#endif //_MEMORY_H_

