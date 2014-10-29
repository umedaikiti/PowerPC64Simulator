#include <string.h>
#include <stdlib.h>
#include "PowerPC64.h"
#include "Memory.h"

//$B%"%/%;%9$7$h$&$H$7$F$$$k%a%b%jNN0h$,$9$G$K3NJ]$5$l$F$$$k$+%j%K%"%5!<%A$GD4$Y$k(B
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

//PowerPC$B$N%a%b%j%"%I%l%9(Baddress$B$N;X$9NN0h$+$i(Bdata$B$K(Bn$B%P%$%H$N%G!<%?$rFI$_9~$`(B
int ReadMemory(memory_t *memory, unsigned long long address, unsigned char *data, int n)
{
	while(n > 0){
		int pagenum = FindPage(memory, address);
		int loweraddress = address & ~PAGE_MASK;
		int readable = PAGE_SIZE - loweraddress;
		int byte_to_read = n < readable ? n : readable;
		if(pagenum == -1){
			memset(data, 0, byte_to_read);
		}
		else{
			memcpy(data, memory->pages[pagenum]+loweraddress, byte_to_read);
		}
		address += byte_to_read;
		data += byte_to_read;
		n -= byte_to_read;
	}
	return 0;
}

//PowerPC$B$N%a%b%j%"%I%l%9(Baddress$B$N;X$9NN0h$K(Bdata$B$+$i(Bn$B%P%$%H$N%G!<%?$r=q$-9~$`(B
int WriteMemory(memory_t *memory, unsigned long long address, const unsigned char *data, int n)
{
	while(n > 0){
		int pagenum = FindPage(memory, address);
		int loweraddress = address & ~PAGE_MASK;
		int writable = PAGE_SIZE - loweraddress;
		int byte_to_write = n < writable ? n : writable;
		if(pagenum == -1){
			pagenum = memory->page_count;
			if(pagenum >= MAX_PAGES){
				return -1;
			}
			void *p = malloc(PAGE_SIZE);
			if(p == NULL){
				return -2;
			}
			memory->pages[pagenum] = p;
			memory->page_count = pagenum + 1;
			memory->page_address[pagenum] = address & PAGE_MASK;
		}
		memcpy(memory->pages[pagenum]+loweraddress, data, byte_to_write);
		address += byte_to_write;
		data += byte_to_write;
		n -= byte_to_write;
	}
	return 0;
}

int ReadMemory8(memory_t *memory, unsigned long long address, unsigned long long *data)
{
	unsigned char cdata[8];
	int i, err;
	err = ReadMemory(memory, address, cdata, 8);
	if(err != 0){
		return err;
	}

#ifndef TARGET_LITTLEENDIAN
	*data = 0;
	for(i=0;i<8;i++){
		*data |= (unsigned long long)cdata[i] << (8*(7-i));
	}
#else
	*data = 0;
	for(i=0;i<8;i++){
		*data |= (unsigned long long)cdata[i] << (8*i);
	}
#endif

	return 0;
}
int ReadMemory4(memory_t *memory, unsigned long long address, unsigned int *data)
{
	unsigned char cdata[4];
	int i, err;
	err = ReadMemory(memory, address, cdata, 4);
	if(err != 0){
		return err;
	}

#ifndef TARGET_LITTLEENDIAN
	*data = 0;
	for(i=0;i<4;i++){
		*data |= (unsigned int)cdata[i] << (8*(3-i));
	}
#else
	*data = 0;
	for(i=0;i<4;i++){
		*data |= (unsigned int)cdata[i] << (8*i);
	}
#endif

	return 0;
}
int ReadMemory2(memory_t *memory, unsigned long long address, unsigned short *data)
{
	unsigned char cdata[2];
	int err;
	err = ReadMemory(memory, address, cdata, 2);
	if(err != 0){
		return err;
	}

#ifndef TARGET_LITTLEENDIAN
	*data = (unsigned short)((unsigned int)cdata[1] | ((unsigned int)cdata[0] << 8));
#else
	*data = (unsigned short)((unsigned int)cdata[0] | ((unsigned int)cdata[1] << 8));
#endif

	return 0;

}
int ReadMemory1(memory_t *memory, unsigned long long address, unsigned char *data)
{
	unsigned char cdata;
	int err;
	err = ReadMemory(memory, address, &cdata, 1);
	if(err != 0){
		return err;
	}
	*data = cdata;
	return 0;
}

int WriteMemory8(memory_t *memory, unsigned long long address, unsigned long long data)
{
	int i, err;
	unsigned char cdata[8];

#ifndef TARGET_LITTLEENDIAN
	for(i=0;i<8;i++){
		cdata[7-i] = (unsigned char)(data & 0xFF);
		data >>= 8;
	}
#else
	for(i=0;i<8;i++){
		cdata[i] = (unsigned char)(data & 0xFF);
		data >>= 8;
	}
#endif

	err = WriteMemory(memory, address, cdata, 8);
	if(err != 0){
		return err;
	}
	return 0;
}
int WriteMemory4(memory_t *memory, unsigned long long address, unsigned int data)
{
	int i, err;
	unsigned char cdata[4];

#ifndef TARGET_LITTLEENDIAN
	for(i=0;i<4;i++){
		cdata[3-i] = (unsigned char)(data & 0xFF);
		data >>= 8;
	}
#else
	for(i=0;i<4;i++){
		cdata[i] = (unsigned char)(data & 0xFF);
		data >>= 8;
	}
#endif

	err = WriteMemory(memory, address, cdata, 4);
	if(err != 0){
		return err;
	}
	return 0;
}
int WriteMemory2(memory_t *memory, unsigned long long address, unsigned short data)
{
	int err;
	unsigned char cdata[2];

#ifndef TARGET_LITTLEENDIAN
	cdata[0] = (unsigned char)((data >> 8) & 0xFF);
	cdata[1] = (unsigned char)(data & 0xFF);
#else
	cdata[0] = (unsigned char)(data & 0xFF);
	cdata[1] = (unsigned char)((data >> 8) & 0xFF);
#endif

	err = WriteMemory(memory, address, cdata, 2);
	if(err != 0){
		return err;
	}
	return 0;
}
int WriteMemory1(memory_t *memory, unsigned long long address, unsigned char data)
{
	int err;
	err = WriteMemory(memory, address, &data, 1);
	if(err != 0){
		return err;
	}
	return 0;
}


