#include <string.h>
#include <stdlib.h>
#include "PowerPC64.h"
#include "Memory.h"

//アクセスしようとしているメモリ領域がすでに確保されているかリニアサーチで調べる
int FindPage(memory_t *memory, unsigned long long address)
{
	int i;
	address &= PAGE_MASK;
	for(i=0;i<memory->page_count;i++){
		if(memory->page_address[i] == address){
			return i;
		}
	}
	return -1;
}

//PowerPCのメモリアドレスaddressの指す領域からdataにnバイトのデータを読み込む
int ReadMemory(ppc64_t *ppc, unsigned long long address, unsigned char *data, int n)
{
	while(n > 0){
		int pagenum = FindPage(&ppc->memory, address);
		int loweraddress = address & ~PAGE_MASK;
		int readable = PAGE_SIZE - loweraddress;
		int byte_to_read = n < readable ? n : readable;
		if(pagenum == -1){
			memset(data, 0, byte_to_read);
		}
		else{
			memcpy(data, ppc->memory.pages[pagenum]+loweraddress, byte_to_read);
		}
		address += byte_to_read;
		data += byte_to_read;
		n -= byte_to_read;
	}
	return 0;
}

//PowerPCのメモリアドレスaddressの指す領域にdataからnバイトのデータを書き込む
int WriteMemory(ppc64_t *ppc, unsigned long long address, const unsigned char *data, int n)
{
	while(n > 0){
		int pagenum = FindPage(&ppc->memory, address);
		int loweraddress = address & ~PAGE_MASK;
		int writable = PAGE_SIZE - loweraddress;
		int byte_to_write = n < writable ? n : writable;
		if(pagenum == -1){
			pagenum = ppc->memory.page_count;
			if(pagenum >= MAX_PAGES){
				return -1;
			}
			void *p = malloc(PAGE_SIZE);
			if(p == NULL){
				return -2;
			}
			ppc->memory.pages[pagenum] = p;
			ppc->memory.page_count = pagenum + 1;
			ppc->memory.page_address[pagenum] = address & PAGE_MASK;
		}
		memcpy(ppc->memory.pages[pagenum]+loweraddress, data, byte_to_write);
		address += byte_to_write;
		data += byte_to_write;
		n -= byte_to_write;
	}
	return 0;
}

int ReadMemory8(ppc64_t *ppc, unsigned long long address, unsigned long long *data)
{
	unsigned char cdata[8];
	int i, err;
	err = ReadMemory(ppc, address, cdata, 8);
	if(err != 0){
		return err;
	}

#ifndef TARGET_LITTLEENDIAN
	*data = 0;
	for(i=0;i<8;i++){
		*data |= cdata[i] << (8*(7-i));
	}
#else
	*data = 0;
	for(i=0;i<8;i++){
		*data |= cdata[i] << (8*i);
	}
#endif

	return 0;
}
int ReadMemory4(ppc64_t *ppc, unsigned long long address, unsigned int *data)
{
	unsigned char cdata[4];
	int i, err;
	err = ReadMemory(ppc, address, cdata, 4);
	if(err != 0){
		return err;
	}

#ifndef TARGET_LITTLEENDIAN
	*data = 0;
	for(i=0;i<4;i++){
		*data |= cdata[i] << (8*(3-i));
	}
#else
	*data = 0;
	for(i=0;i<4;i++){
		*data |= cdata[i] << (8*i);
	}
#endif

	return 0;
}
int ReadMemory2(ppc64_t *ppc, unsigned long long address, unsigned short *data)
{
	unsigned char cdata[2];
	int err;
	err = ReadMemory(ppc, address, cdata, 2);
	if(err != 0){
		return err;
	}

#ifndef TARGET_LITTLEENDIAN
	*data = cdata[1] | (cdata[0] << 8);
#else
	*data = cdata[0] | (cdata[1] << 8);
#endif

	return 0;

}
int ReadMemory1(ppc64_t *ppc, unsigned long long address, unsigned char *data)
{
	unsigned char cdata;
	int err;
	err = ReadMemory(ppc, address, &cdata, 1);
	if(err != 0){
		return err;
	}
	*data = cdata;
	return 0;
}

int WriteMemory8(ppc64_t *ppc, unsigned long long address, unsigned long long data)
{
	int i, err;
	unsigned char cdata[8];

#ifndef TARGET_LITTLEENDIAN
	for(i=0;i<8;i++){
		cdata[7-i] = data & 0xFF;
		data >>= 8;
	}
#else
	for(i=0;i<8;i++){
		cdata[i] = data & 0xFF;
		data >>= 8;
	}
#endif

	err = WriteMemory(ppc, address, cdata, 8);
	if(err != 0){
		return err;
	}
	return 0;
}
int WriteMemory4(ppc64_t *ppc, unsigned long long address, unsigned int data)
{
	int i, err;
	unsigned char cdata[4];

#ifndef TARGET_LITTLEENDIAN
	for(i=0;i<4;i++){
		cdata[3-i] = data & 0xFF;
		data >>= 8;
	}
#else
	for(i=0;i<4;i++){
		cdata[i] = data & 0xFF;
		data >>= 8;
	}
#endif

	err = WriteMemory(ppc, address, cdata, 4);
	if(err != 0){
		return err;
	}
	return 0;
}
int WriteMemory2(ppc64_t *ppc, unsigned long long address, unsigned short data)
{
	int err;
	unsigned char cdata[2];

#ifndef TARGET_LITTLEENDIAN
	cdata[0] = (data >> 8) & 0xFF;
	cdata[1] = data & 0xFF;
#else
	cdata[0] = data & 0xFF;
	cdata[1] = (data >> 8) & 0xFF;
#endif

	err = WriteMemory(ppc, address, cdata, 2);
	if(err != 0){
		return err;
	}
	return 0;
}
int WriteMemory1(ppc64_t *ppc, unsigned long long address, unsigned char data)
{
	int err;
	err = WriteMemory(ppc, address, &data, 1);
	if(err != 0){
		return err;
	}
	return 0;
}


