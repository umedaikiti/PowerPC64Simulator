#include "PowerPC64.h"
#include "Memory.h"
#include "InstructionFormat.h"
#include "Instructions.h"

#define EXTS32(x) (((long long)(int)(x) << 32) >> 32)
#define EXTS16(x) (((long long)(short)(x) << 48) >> 48)
#define EXTS8(x) (((long long)(char)(x) << 56) >> 56)
#define MSB(x) (((unsigned long long)(x)) >> 63)
#define OV(a,b,c) ((MSB(a) ^ MSB(c)) & (MSB(b) ^ MSB(c)))
#define CA(a, b, c) carry(a, b, c)

/* dform template
int (ppc64_t *ppc, inst_t inst){
	unsigned long long rt = ppc->regs.GPR[inst.dform.rt];
	unsigned long long imm = inst.dform.imm;
	ppc->regs.GPR[inst.dform.ra] = ;
	return RET_SUCCESS;
}
*/

static inline unsigned long long rotl64(unsigned long long x, unsigned int y)
{
	if(y == 0){
		return x;
	}
	else{
		return (x << y) | (x >> (64 - y));
	}
}
static inline unsigned long long rotl32(unsigned int x, unsigned int y)
{
	unsigned long long xx = x | (((unsigned long long)x) << 32);
	return rotl64(xx, y);
}

static inline unsigned int carry(unsigned long long a, unsigned long long b, unsigned int c)
{
	unsigned long long al, ah, bl, bh;
	al = a & 0xFFFFFFFF;
	bl = b & 0xFFFFFFFF;
	ah = a >> 32;
	bh = b >> 32;
	return (ah + bh + ((al + bl + c) >> 32)) >> 32;
}

static inline unsigned int condition(ppc64_t *ppc, long long a, long long b){
	unsigned int cr = 0;
	if(a < b){
		cr |= 8;
	}
	else if(a > b){
		cr |= 4;
	}
	else{
		cr |= 2;
	}

	cr |= ppc->regs.XER.field.so;

	return cr;
}

int addi(ppc64_t *ppc, inst_t inst)
{
	if(inst.dform.ra == 0){
		ppc->regs.GPR[inst.dform.rt] = EXTS16(inst.dform.imm);
	}
	else{
		ppc->regs.GPR[inst.dform.rt] = ppc->regs.GPR[inst.dform.ra] + EXTS16(inst.dform.imm);
	}
	return RET_SUCCESS;
}

int addis(ppc64_t *ppc, inst_t inst)
{
	if(inst.dform.ra == 0){
		ppc->regs.GPR[inst.dform.rt] = EXTS16(inst.dform.imm) << 16;
	}
	else{
		ppc->regs.GPR[inst.dform.rt] = ppc->regs.GPR[inst.dform.ra] + (EXTS16(inst.dform.imm) << 16);
	}
	return RET_SUCCESS;
}

/* xoform template
int (ppc64_t *ppc, inst_t inst){
	unsigned long long ra, rb, rt;
	ra = ppc->regs.GPR[inst.xoform.ra];
	rb = ppc->regs.GPR[inst.xoform.rb];
	rt = ra + rb;
	ppc->regs.GPR[inst.xoform.rt] = rt;
	if(inst.xoform.oe){
		ppc->regs.XER.field.so |= ppc->regs.XER.field.ov = OV(ra, rb, rt);
	}
	if(inst.xoform.rc){
		ppc->regs.CR.field.cr0 = condition(ppc, rt, 0);
	}
}
*/

int add(ppc64_t *ppc, inst_t inst){
	unsigned long long ra, rb, rt;
	ra = ppc->regs.GPR[inst.xoform.ra];
	rb = ppc->regs.GPR[inst.xoform.rb];
	rt = ra + rb;
	ppc->regs.GPR[inst.xoform.rt] = rt;
	if(inst.xoform.oe){
		ppc->regs.XER.field.so |= ppc->regs.XER.field.ov = OV(ra, rb, rt);
	}
	if(inst.xoform.rc){
		ppc->regs.CR.field.cr0 = condition(ppc, rt, 0);
	}
	return RET_SUCCESS;
}

int subf(ppc64_t *ppc, inst_t inst){
	unsigned long long ra, rb, rt;
	ra = ppc->regs.GPR[inst.xoform.ra];
	rb = ppc->regs.GPR[inst.xoform.rb];
	rt = ~ra + rb + 1;
	ppc->regs.GPR[inst.xoform.rt] = rt;
	if(inst.xoform.oe){
		ppc->regs.XER.field.ov = OV(~ra + 1, rb, rt);
	}
	if(inst.xoform.rc){
		ppc->regs.CR.field.cr0 = condition(ppc, rt, 0);
	}
	return RET_SUCCESS;
}


int andi(ppc64_t *ppc, inst_t inst){
	unsigned long long result;
	result = ppc->regs.GPR[inst.dform.ra] = ppc->regs.GPR[inst.dform.rt] & inst.dform.imm;
	ppc->regs.CR.field.cr0 = condition(ppc, result, 0);
	return RET_SUCCESS;
}
int andis(ppc64_t *ppc, inst_t inst){
	unsigned long long result;
	result = ppc->regs.GPR[inst.dform.ra] = ppc->regs.GPR[inst.dform.rt] & (inst.dform.imm << 16);
	ppc->regs.CR.field.cr0 = condition(ppc, result, 0);
	return RET_SUCCESS;
}
int ori(ppc64_t *ppc, inst_t inst){
	ppc->regs.GPR[inst.dform.ra] = ppc->regs.GPR[inst.dform.rt] | inst.dform.imm;
	return RET_SUCCESS;
}
int oris(ppc64_t *ppc, inst_t inst){
	ppc->regs.GPR[inst.dform.ra] = ppc->regs.GPR[inst.dform.rt] | (inst.dform.imm << 16);
	return RET_SUCCESS;
}
int xori(ppc64_t *ppc, inst_t inst){
	ppc->regs.GPR[inst.dform.ra] = ppc->regs.GPR[inst.dform.rt] ^ inst.dform.imm;
	return RET_SUCCESS;
}
int xoris(ppc64_t *ppc, inst_t inst){
	ppc->regs.GPR[inst.dform.ra] = ppc->regs.GPR[inst.dform.rt] ^ (inst.dform.imm << 16);
	return RET_SUCCESS;
}


/*
 * addic (12)
 * addic. (13)
*/
int addic(ppc64_t *ppc, inst_t inst)
{
	unsigned long long ra = ppc->regs.GPR[inst.dform.ra];
	unsigned long long si = EXTS16(inst.dform.imm);
	ppc->regs.XER.field.ca = CA(ra, si, 0);
	ppc->regs.GPR[inst.dform.rt] = ra + si;
	if(inst.dform.opcd == 13){
		ppc->regs.CR.field.cr0 = condition(ppc, ra+si, 0);
	}
	return RET_SUCCESS;
}

int subfic(ppc64_t *ppc, inst_t inst)
{
	unsigned long long ra = ppc->regs.GPR[inst.dform.ra];
	unsigned long long si = EXTS16(inst.dform.imm);
	ppc->regs.XER.field.ca = CA(~ra, si, 1);
	ppc->regs.GPR[inst.dform.rt] = ~ra + si + 1;
	return RET_SUCCESS;
}

/* xform template
int (ppc64_t *ppc, inst_t inst){
	unsigned long long rs = ppc->regs.GPR[inst.xform.rt];
	unsigned long long rb = ppc->regs.GPR[inst.xform.rt];
	unsigned long long result = ;
	ppc->regs.GPR[inst.xform.ra] = result;
	if(inst.xform.rc){
		ppc->regs.CR.field.cr0 = condition(ppc, result, 0);
	}
	return RET_SUCCESS;
}
*/

static int FullAdderXO(ppc64_t *ppc, inst_t inst, unsigned long long a, unsigned long long b, unsigned long long carryin)
{
	unsigned long long rt;
	rt = a + b + carryin;
	ppc->regs.GPR[inst.xoform.rt] = rt;
	ppc->regs.XER.field.ca = CA(a, b, carryin);
	if(inst.xoform.oe){
		ppc->regs.XER.field.so |= ppc->regs.XER.field.ov = OV(a, b, rt);
	}
	if(inst.xoform.rc){
		ppc->regs.CR.field.cr0 = condition(ppc, rt, 0);
	}
	return RET_SUCCESS;
}

int adde(ppc64_t *ppc, inst_t inst){
	unsigned long long ra, rb;
	ra = ppc->regs.GPR[inst.xoform.ra];
	rb = ppc->regs.GPR[inst.xoform.rb];
	return FullAdderXO(ppc, inst, ra, rb, ppc->regs.XER.field.ca);
}

int addc(ppc64_t *ppc, inst_t inst)
{
	unsigned long long ra, rb;
	ra = ppc->regs.GPR[inst.xoform.ra];
	rb = ppc->regs.GPR[inst.xoform.rb];
	return FullAdderXO(ppc, inst, ra, rb, 0);
}
int subfc(ppc64_t *ppc, inst_t inst)
{
	unsigned long long ra, rb;
	ra = ppc->regs.GPR[inst.xoform.ra];
	rb = ppc->regs.GPR[inst.xoform.rb];
	return FullAdderXO(ppc, inst, ~ra, rb, 1);
}
int subfe(ppc64_t *ppc, inst_t inst)
{
	unsigned long long ra, rb;
	ra = ppc->regs.GPR[inst.xoform.ra];
	rb = ppc->regs.GPR[inst.xoform.rb];
	return FullAdderXO(ppc, inst, ~ra, rb, ppc->regs.XER.field.ca);
}
int addme(ppc64_t *ppc, inst_t inst)
{
	unsigned long long ra;
	ra = ppc->regs.GPR[inst.xoform.ra];
	return FullAdderXO(ppc, inst, ra, -1ll, ppc->regs.XER.field.ca);
}
int subfme(ppc64_t *ppc, inst_t inst)
{
	unsigned long long ra;
	ra = ppc->regs.GPR[inst.xoform.ra];
	return FullAdderXO(ppc, inst, ~ra, -1ll, ppc->regs.XER.field.ca);
}
int addze(ppc64_t *ppc, inst_t inst)
{
	unsigned long long ra;
	ra = ppc->regs.GPR[inst.xoform.ra];
	return FullAdderXO(ppc, inst, ra, 0, ppc->regs.XER.field.ca);
}
int subfze(ppc64_t *ppc, inst_t inst)
{
	unsigned long long ra;
	ra = ppc->regs.GPR[inst.xoform.ra];
	return FullAdderXO(ppc, inst, ~ra, 0, ppc->regs.XER.field.ca);
}
int neg(ppc64_t *ppc, inst_t inst)
{
	unsigned long long ra, rt;
	ra = ppc->regs.GPR[inst.xoform.ra];
	rt = ~ra + 1;
	if(inst.xoform.oe){
		ppc->regs.XER.field.so |= ppc->regs.XER.field.ov = (ra == (1ull << 63) ? 1 : 0);
	}
	if(inst.xoform.rc){
		ppc->regs.CR.field.cr0 = condition(ppc, rt, 0);
	}
	return RET_SUCCESS;
}
int and(ppc64_t *ppc, inst_t inst){
	unsigned long long rs = ppc->regs.GPR[inst.xform.rt];
	unsigned long long rb = ppc->regs.GPR[inst.xform.rt];
	unsigned long long result = rs & rb;
	ppc->regs.GPR[inst.xform.ra] = result;
	if(inst.xform.rc){
		ppc->regs.CR.field.cr0 = condition(ppc, result, 0);
	}
	return RET_SUCCESS;
}
int or(ppc64_t *ppc, inst_t inst){
	unsigned long long rs = ppc->regs.GPR[inst.xform.rt];
	unsigned long long rb = ppc->regs.GPR[inst.xform.rt];
	unsigned long long result = rs | rb;
	ppc->regs.GPR[inst.xform.ra] = result;
	if(inst.xform.rc){
		ppc->regs.CR.field.cr0 = condition(ppc, result, 0);
	}
	return RET_SUCCESS;
}
int xor(ppc64_t *ppc, inst_t inst){
	unsigned long long rs = ppc->regs.GPR[inst.xform.rt];
	unsigned long long rb = ppc->regs.GPR[inst.xform.rt];
	unsigned long long result = rs ^ rb;
	ppc->regs.GPR[inst.xform.ra] = result;
	if(inst.xform.rc){
		ppc->regs.CR.field.cr0 = condition(ppc, result, 0);
	}
	return RET_SUCCESS;
}
int nand(ppc64_t *ppc, inst_t inst){
	unsigned long long rs = ppc->regs.GPR[inst.xform.rt];
	unsigned long long rb = ppc->regs.GPR[inst.xform.rt];
	unsigned long long result = ~(rs & rb);
	ppc->regs.GPR[inst.xform.ra] = result;
	if(inst.xform.rc){
		ppc->regs.CR.field.cr0 = condition(ppc, result, 0);
	}
	return RET_SUCCESS;
}
int nor(ppc64_t *ppc, inst_t inst){
	unsigned long long rs = ppc->regs.GPR[inst.xform.rt];
	unsigned long long rb = ppc->regs.GPR[inst.xform.rt];
	unsigned long long result = ~(rs | rb);
	ppc->regs.GPR[inst.xform.ra] = result;
	if(inst.xform.rc){
		ppc->regs.CR.field.cr0 = condition(ppc, result, 0);
	}
	return RET_SUCCESS;
}
int eqv(ppc64_t *ppc, inst_t inst){
	unsigned long long rs = ppc->regs.GPR[inst.xform.rt];
	unsigned long long rb = ppc->regs.GPR[inst.xform.rt];
	unsigned long long result = ~(rs ^ rb);
	ppc->regs.GPR[inst.xform.ra] = result;
	if(inst.xform.rc){
		ppc->regs.CR.field.cr0 = condition(ppc, result, 0);
	}
	return RET_SUCCESS;
}
int andc(ppc64_t *ppc, inst_t inst){
	unsigned long long rs = ppc->regs.GPR[inst.xform.rt];
	unsigned long long rb = ppc->regs.GPR[inst.xform.rt];
	unsigned long long result = rs & ~rb;
	ppc->regs.GPR[inst.xform.ra] = result;
	if(inst.xform.rc){
		ppc->regs.CR.field.cr0 = condition(ppc, result, 0);
	}
	return RET_SUCCESS;
}
int orc(ppc64_t *ppc, inst_t inst){
	unsigned long long rs = ppc->regs.GPR[inst.xform.rt];
	unsigned long long rb = ppc->regs.GPR[inst.xform.rt];
	unsigned long long result = rs | ~rb;
	ppc->regs.GPR[inst.xform.ra] = result;
	if(inst.xform.rc){
		ppc->regs.CR.field.cr0 = condition(ppc, result, 0);
	}
	return RET_SUCCESS;
}
int extsb(ppc64_t *ppc, inst_t inst){
	unsigned long long rs = ppc->regs.GPR[inst.xform.rt];
	unsigned long long result = EXTS8(rs & 0xFF);
	ppc->regs.GPR[inst.xform.ra] = result;
	if(inst.xform.rc){
		ppc->regs.CR.field.cr0 = condition(ppc, result, 0);
	}
	return RET_SUCCESS;
}

int extsh(ppc64_t *ppc, inst_t inst)
{
	unsigned long long rs = ppc->regs.GPR[inst.xform.rt];
	unsigned long long result = EXTS16(rs & 0xFFFF);
	ppc->regs.GPR[inst.xform.ra] = result;
	if(inst.xform.rc){
		ppc->regs.CR.field.cr0 = condition(ppc, result, 0);
	}
	return RET_SUCCESS;
}

int cntlzw(ppc64_t *ppc, inst_t inst)
{
	unsigned long long rs = ppc->regs.GPR[inst.xform.rt];
	long long result = 31;
	for(;result>=0;result--){
		if((rs >> result) & 1) break;
	}
	result = 31 - result;
	ppc->regs.GPR[inst.xform.ra] = result;
	if(inst.xform.rc){
		ppc->regs.CR.field.cr0 = condition(ppc, result, 0);
	}
	return RET_SUCCESS;
}

int cmpb(ppc64_t *ppc, inst_t inst)
{
	unsigned long long rs = ppc->regs.GPR[inst.xform.rt];
	unsigned long long rb = ppc->regs.GPR[inst.xform.rb];
	unsigned long long result;
	int i;
	unsigned char *rsp, *rbp, *res_p;
	rsp = (unsigned char*)&rs;
	rbp = (unsigned char*)&rb;
	res_p = (unsigned char*)&result;
	for(i=0;i<8;i++){
		if(rsp[i] == rbp[i]){
			res_p[i] = 0;
		}
		else{
			res_p[i] = 0xFF;
		}
	}
	ppc->regs.GPR[inst.xform.ra] = result;
	return RET_SUCCESS;
}

#define MASK1 (0x5555555555555555)
#define MASK2 (0x3333333333333333)
#define MASK4 (0x0f0f0f0f0f0f0f0f)
#define MASK8 (0x00ff00ff00ff00ff)
#define MASK16 (0x0000ffff0000ffff)
#define MASK32 (0x00000000ffffffff)

int popcntb(ppc64_t *ppc, inst_t inst)
{
	unsigned long long rs = ppc->regs.GPR[inst.xform.rt];
	unsigned long long result;
	result = (rs & MASK1) + ((rs >> 1) & MASK1);
	result = (result & MASK2) + ((result >> 2) & MASK2);
	result = (result & MASK4) + ((result >> 4) & MASK4);
	ppc->regs.GPR[inst.xform.ra] = result;
	return RET_SUCCESS;
}

int popcntw(ppc64_t *ppc, inst_t inst)
{
	unsigned long long rs = ppc->regs.GPR[inst.xform.rt];
	unsigned long long result;
	result = (rs & MASK1) + ((rs >> 1) & MASK1);
	result = (result & MASK2) + ((result >> 2) & MASK2);
	result = (result & MASK4) + ((result >> 4) & MASK4);
	result = (result & MASK8) + ((result >> 8) & MASK8);
	result = (result & MASK16) + ((result >> 16) & MASK16);
	ppc->regs.GPR[inst.xform.ra] = result;
	return RET_SUCCESS;
}


int cmpi(ppc64_t *ppc, inst_t inst)
{
	long long ra = ppc->regs.GPR[inst.dform.ra];
	unsigned int bf = inst.dform.rt >> 2;
	unsigned int l = inst.dform.rt & 1;
	long long si = EXTS16(inst.dform.imm);
	long long a = (l == 0 ? EXTS32(ra & 0xFFFFFFFF) : ra);
	unsigned int c = condition(ppc, a, si);
	setCR(ppc, bf, c);
	return RET_SUCCESS;
}

int cmp(ppc64_t *ppc, inst_t inst)
{
	unsigned long long ra = ppc->regs.GPR[inst.xform.ra];
	unsigned long long rb = ppc->regs.GPR[inst.xform.rb];
	unsigned int bf = inst.xform.rt >> 2;
	unsigned int l = inst.xform.rt & 1;
	long long a = l == 0 ? EXTS32(ra & 0xFFFFFFFF) : (long long)ra;
	long long b = l == 0 ? EXTS32(rb & 0xFFFFFFFF) : (long long)rb;
	unsigned int c = condition(ppc, a, b);
	setCR(ppc, bf, c);
	return RET_SUCCESS;
}

int cmpli(ppc64_t *ppc, inst_t inst)
{
	unsigned int bf = inst.dform.rt >> 2;
	unsigned int l = inst.dform.rt & 1;
	unsigned long long ra = ppc->regs.GPR[inst.dform.ra];
	unsigned long long a = l == 0 ? (ra & 0xFFFFFFFF) : ra;
	unsigned long long ui = inst.dform.imm;
	unsigned int c = ppc->regs.XER.field.so;
	if(a < ui) c |= 0x8;
	else if(a > ui) c |= 0x4;
	else c |= 0x2;
	setCR(ppc, bf, c);
	return RET_SUCCESS;
}

int cmpl(ppc64_t *ppc, inst_t inst)
{
	unsigned long long ra = ppc->regs.GPR[inst.xform.ra];
	unsigned long long rb = ppc->regs.GPR[inst.xform.rb];
	unsigned int bf = inst.xform.rt >> 2;
	unsigned int l = inst.xform.rt & 1;
	unsigned long long a = l == 0 ? (ra & 0xFFFFFFFF) : ra;
	unsigned long long b = l == 0 ? (rb & 0xFFFFFFFF) : rb;
	unsigned int c = ppc->regs.XER.field.so;
	if(a < b) c |= 0x8;
	else if(a > b) c |= 0x4;
	else c |= 0x2;
	setCR(ppc, bf, c);
	return RET_SUCCESS;
}

int slw(ppc64_t *ppc, inst_t inst)
{
	unsigned long long rb = ppc->regs.GPR[inst.xform.rb];
	unsigned long long rs = ppc->regs.GPR[inst.xform.rt];
	unsigned int result = rs & 0xFFFFFFFF;
	result <<= rb & 0x1F;
	if((rb >> 5) & 1){
		result = 0;
	}
	ppc->regs.GPR[inst.xform.ra] = result;
	if(inst.xform.rc){
		ppc->regs.CR.field.cr0 = condition(ppc, result, 0);
	}
	return RET_SUCCESS;
}
int srw(ppc64_t *ppc, inst_t inst)
{
	unsigned long long rb = ppc->regs.GPR[inst.xform.rb];
	unsigned long long rs = ppc->regs.GPR[inst.xform.rt];
	unsigned int result = rs & 0xFFFFFFFF;
	result >>= rb & 0x1F;
	if((rb >> 5) & 1){
		result = 0;
	}
	ppc->regs.GPR[inst.xform.ra] = result;
	if(inst.xform.rc){
		ppc->regs.CR.field.cr0 = condition(ppc, result, 0);
	}
	return RET_SUCCESS;
}

int srawi(ppc64_t *ppc, inst_t inst)
{
	unsigned int n = inst.xform.rb;
	int rs = (int)ppc->regs.GPR[inst.xform.rt];
	long long result = (long long)(rs >> n);
	unsigned int ca = (rs >> 31) & 1;
	ca &= (rs & ((1 << n) - 1)) == 0 ? 0 : 1;
	ppc->regs.XER.field.ca = ca;
	ppc->regs.GPR[inst.xform.ra] = (unsigned long long)result;
	if(inst.xform.rc){
		ppc->regs.CR.field.cr0 = condition(ppc, result, 0);
	}
	return RET_SUCCESS;
}

int sraw(ppc64_t *ppc, inst_t inst)
{
/* caが分からない
	unsigned long long rb = ppc->regs.GPR[inst.xform.rb];
	unsigned int n = rb & 0x1F;
	int rs = (int)ppc->regs.GPR[inst.xform.rt];
	long long result = (long long)(rs >> n);
	unsigned int s = (rs >> 31) & 1;
	if((rb & 0x20) != 0){
		result = s == 1 ? 0xFFFFFFFFFFFFFFFF : 0;
	}
	unsigned int ca = s & ((rs & ((1 << n) - 1)) == 0 ? 0 : 1);
	ppc->regs.GPR[inst.xform.ra] = (unsigned long long)result;
	ppc->regs.XER.field.ca = ca;
	if(inst.xform.rc){
		ppc->regs.CR.field.cr0 = condition(ppc, result, 0);
	}
	return RET_SUCCESS;
*/
	return RET_NOTIMPLEMENTED;
}

int mtspr(ppc64_t *ppc, inst_t inst)
{
	unsigned long long rs = ppc->regs.GPR[inst.xfxform.rt];
	unsigned int spr = inst.xfxform.r;
	unsigned int n = ((spr & 0x1f) << 5) | (spr >> 5);
	switch(n){
	case 1:
		ppc->regs.XER.raw = rs;
		break;
	case 8:
		ppc->regs.LR = rs;
		break;
	case 9:
		ppc->regs.CTR = rs;
		break;
	default:
		return RET_NOTIMPLEMENTED;
	}
	return RET_SUCCESS;
}

int mfspr(ppc64_t *ppc, inst_t inst)
{
	unsigned int spr = inst.xfxform.r;
	unsigned int n = ((spr & 0x1f) << 5) | (spr >> 5);
	switch(n){
	case 1:
		ppc->regs.GPR[inst.xfxform.rt] = ppc->regs.XER.raw;
		break;
	case 8:
		ppc->regs.GPR[inst.xfxform.rt] = ppc->regs.LR;
		break;
	case 9:
		ppc->regs.GPR[inst.xfxform.rt] = ppc->regs.CTR;
		break;
	default:
		return RET_NOTIMPLEMENTED;
	}
	return RET_SUCCESS;
}
int mtcrf(ppc64_t *ppc, inst_t inst)
{
	return RET_NOTIMPLEMENTED;
}
int mfcr(ppc64_t *ppc, inst_t inst)
{
	ppc->regs.GPR[inst.xfxform.rt] = ppc->regs.CR.raw;
	return RET_SUCCESS;
}
int mtocrf(ppc64_t *ppc, inst_t inst)
{
	return RET_NOTIMPLEMENTED;
}
int mfocrf(ppc64_t *ppc, inst_t inst)
{
	return RET_NOTIMPLEMENTED;
}
int mcrxr(ppc64_t *ppc, inst_t inst)
{
	return RET_NOTIMPLEMENTED;
}
int mtdcrux(ppc64_t *ppc, inst_t inst)
{
	return RET_NOTIMPLEMENTED;
}
int mfdcrux(ppc64_t *ppc, inst_t inst)
{
	return RET_NOTIMPLEMENTED;
}

int b(ppc64_t *ppc, inst_t inst)
{
	long long nia = ((long long)inst.iform.li << 40) >> 38;
	if(inst.iform.aa){
		ppc->nia = nia;
	}
	else{
		ppc->nia = nia + ppc->cia;
	}

	if(inst.iform.lk){
		ppc->regs.LR = ppc->cia + 4;
	}
	return RET_SUCCESS;
}

int bc(ppc64_t *ppc, inst_t inst)
{
	unsigned int bo4 = inst.bform.bo;
	unsigned int bo3 = bo4 >> 1;
	unsigned int bo2 = bo3 >> 1;
	unsigned int bo1 = bo2 >> 1;
	unsigned int bo0 = bo1 >> 1;
	unsigned int ctr_ok = bo2 & 1;
	unsigned int cond_ok;
	bo0 &= 1;
	bo1 &= 1;
	bo2 &= 1;
	bo3 &= 1;
	bo4 &= 1;

	if(!bo2){
		ppc->regs.CTR--;
	}
	ctr_ok |= (ppc->regs.CTR != 0 ? 1 : 0) ^ bo3;
	cond_ok = bo0 | ~(((ppc->regs.CR.raw >> (31 - inst.bform.bi)) & 1) ^ bo1);
	ctr_ok &= 1;
	cond_ok &= 1;
	if(ctr_ok & cond_ok){
		if(inst.bform.aa){
			ppc->nia = EXTS16(inst.bform.bd << 2);
		}
		else{
			ppc->nia = ppc->cia + EXTS16(inst.bform.bd << 2);
		}
	}
	if(inst.bform.lk){
		ppc->regs.LR = ppc->cia + 4;
	}

	return RET_SUCCESS;
}
int bclr(ppc64_t *ppc, inst_t inst)
{
	unsigned int bo4 = inst.bform.bo;
	unsigned int bo3 = bo4 >> 1;
	unsigned int bo2 = bo3 >> 1;
	unsigned int bo1 = bo2 >> 1;
	unsigned int bo0 = bo1 >> 1;
	unsigned int ctr_ok = bo2 & 1;
	unsigned int cond_ok;
	bo0 &= 1;
	bo1 &= 1;
	bo2 &= 1;
	bo3 &= 1;
	bo4 &= 1;

	if(!bo2){
		ppc->regs.CTR--;
	}
	ctr_ok |= (ppc->regs.CTR != 0 ? 1 : 0) ^ bo3;
	cond_ok = bo0 | ~(((ppc->regs.CR.raw >> (31 - inst.bform.bi)) & 1) ^ bo1);
	ctr_ok &= 1;
	cond_ok &= 1;
	if(ctr_ok & cond_ok){
		ppc->nia = ppc->regs.LR & ~0x3ull;
	}
	if(inst.bform.lk){
		ppc->regs.LR = ppc->cia + 4;
	}

	return RET_SUCCESS;
}
int bcctr(ppc64_t *ppc, inst_t inst)
{
	unsigned int bo = inst.xlform.bt;
	unsigned int bi = inst.xlform.ba;
	unsigned int crbi = (ppc->regs.CR.raw >> (31 - bi)) & 1;
	unsigned int bo4 = bo;
	unsigned int bo3 = bo4 >> 1;
	unsigned int bo2 = bo3 >> 1;
	unsigned int bo1 = bo2 >> 1;
	unsigned int bo0 = bo1 >> 1;
	bo0 &= 1;
	bo1 &= 1;
	bo2 &= 1;
	bo3 &= 1;
	bo4 &= 1;
	unsigned int cond_ok = bo0 | (crbi == bo1 ? 1 : 0);
	if(cond_ok){
		ppc->nia = ppc->regs.CTR & ~3ull;
	}
	if(inst.xlform.lk){
		ppc->regs.LR = ppc->cia + 4;
	}
	return RET_SUCCESS;
}

int crand(ppc64_t *ppc, inst_t inst)
{
	unsigned long long cr = ppc->regs.CR.raw;
	unsigned long long crba = (cr >> (31 - inst.xlform.ba)) & 1;
	unsigned long long crbb = (cr >> (31 - inst.xlform.bb)) & 1;
	cr &= ~(1ull << (31 - inst.xlform.bt));
	cr |= (crba & crbb) << (31 - inst.xlform.bt);
	ppc->regs.CR.raw = cr;
	return RET_SUCCESS;
}
int crnand(ppc64_t *ppc, inst_t inst)
{
	unsigned long long cr = ppc->regs.CR.raw;
	unsigned long long crba = cr >> (31 - inst.xlform.ba);
	unsigned long long crbb = cr >> (31 - inst.xlform.bb);
	unsigned long long crbt = (~(crba & crbb)) & 1;
	cr &= ~(1ull << (31 - inst.xlform.bt));
	cr |= crbt << (31 - inst.xlform.bt);
	ppc->regs.CR.raw = cr;
	return RET_SUCCESS;
}
int cror(ppc64_t *ppc, inst_t inst)
{
	unsigned long long cr = ppc->regs.CR.raw;
	unsigned long long crba = (cr >> (31 - inst.xlform.ba)) & 1;
	unsigned long long crbb = (cr >> (31 - inst.xlform.bb)) & 1;
	cr &= ~(1ull << (31 - inst.xlform.bt));
	cr |= (crba | crbb) << (31 - inst.xlform.bt);
	ppc->regs.CR.raw = cr;
	return RET_SUCCESS;
}
int crxor(ppc64_t *ppc, inst_t inst)
{
	unsigned long long cr = ppc->regs.CR.raw;
	unsigned long long crba = cr >> (31 - inst.xlform.ba);
	unsigned long long crbb = cr >> (31 - inst.xlform.bb);
	unsigned long long crbt = (crba ^ crbb) & 1;
	cr &= ~(1ull << (31 - inst.xlform.bt));
	cr |= crbt << (31 - inst.xlform.bt);
	ppc->regs.CR.raw = cr;
	return RET_SUCCESS;
}
int crnor(ppc64_t *ppc, inst_t inst)
{
	unsigned long long cr = ppc->regs.CR.raw;
	unsigned long long crba = cr >> (31 - inst.xlform.ba);
	unsigned long long crbb = cr >> (31 - inst.xlform.bb);
	unsigned long long crbt = (~(crba | crbb)) & 1;
	cr &= ~(1ull << (31 - inst.xlform.bt));
	cr |= crbt << (31 - inst.xlform.bt);
	ppc->regs.CR.raw = cr;
	return RET_SUCCESS;
}
int creqv(ppc64_t *ppc, inst_t inst)
{
	unsigned long long cr = ppc->regs.CR.raw;
	unsigned long long crba = cr >> (31 - inst.xlform.ba);
	unsigned long long crbb = cr >> (31 - inst.xlform.bb);
	unsigned long long crbt = (~(crba ^ crbb)) & 1;
	cr &= ~(1ull << (31 - inst.xlform.bt));
	cr |= crbt << (31 - inst.xlform.bt);
	ppc->regs.CR.raw = cr;
	return RET_SUCCESS;
}
int crandc(ppc64_t *ppc, inst_t inst)
{
	unsigned long long cr = ppc->regs.CR.raw;
	unsigned long long crba = cr >> (31 - inst.xlform.ba);
	unsigned long long crbb = cr >> (31 - inst.xlform.bb);
	unsigned long long crbt = (crba & ~crbb) & 1;
	cr &= ~(1ull << (31 - inst.xlform.bt));
	cr |= crbt << (31 - inst.xlform.bt);
	ppc->regs.CR.raw = cr;
	return RET_SUCCESS;
}
int crorc(ppc64_t *ppc, inst_t inst)
{
	unsigned long long cr = ppc->regs.CR.raw;
	unsigned long long crba = cr >> (31 - inst.xlform.ba);
	unsigned long long crbb = cr >> (31 - inst.xlform.bb);
	unsigned long long crbt = (crba | ~crbb) & 1;
	cr &= ~(1ull << (31 - inst.xlform.bt));
	cr |= crbt << (31 - inst.xlform.bt);
	ppc->regs.CR.raw = cr;
	return RET_SUCCESS;
}
int mcrf(ppc64_t *ppc, inst_t inst)
{
	return RET_NOTIMPLEMENTED;
}

int opcd31(ppc64_t *ppc, inst_t inst)
{
	switch(inst.xoform.xo){
	case OPCD31_XO_ADD:
		return add(ppc, inst);
	case OPCD31_XO_SUBF:
		return subf(ppc, inst);
	case OPCD31_XO_ADDC:
		return addc(ppc, inst);
	case OPCD31_XO_SUBFC:
		return subfc(ppc, inst);
	case OPCD31_XO_ADDE:
		return adde(ppc, inst);
	case OPCD31_XO_SUBFE:
		return subfe(ppc, inst);
	case OPCD31_XO_ADDME:
		return addme(ppc, inst);
	case OPCD31_XO_SUBFME:
		return subfme(ppc, inst);
	case OPCD31_XO_ADDZE:
		return addze(ppc, inst);
	case OPCD31_XO_SUBFZE:
		return subfze(ppc, inst);
	case OPCD31_XO_NEG:
		return neg(ppc, inst);
	default:
		break;
	}
	switch(inst.xform.xo){
	case OPCD31_X_AND:
		return and(ppc, inst);
	case OPCD31_X_OR:
		return or(ppc, inst);
	case OPCD31_X_XOR:
		return xor(ppc, inst);
	case OPCD31_X_NAND:
		return nand(ppc, inst);
	case OPCD31_X_NOR:
		return nor(ppc, inst);
	case OPCD31_X_EQV:
		return eqv(ppc, inst);
	case OPCD31_X_ANDC:
		return andc(ppc, inst);
	case OPCD31_X_ORC:
		return orc(ppc, inst);
	case OPCD31_X_EXTSB:
		return extsb(ppc, inst);
	case OPCD31_X_EXTSH:
		return extsh(ppc, inst);
	case OPCD31_X_CNTLZW:
		return cntlzw(ppc, inst);
	case OPCD31_X_CMPB:
		return cmpb(ppc, inst);
	case OPCD31_X_POPCNTB:
		return popcntb(ppc, inst);
	case OPCD31_X_POPCNTW:
		return popcntw(ppc, inst);
	case OPCD31_X_PRTYD:
		return prtyd(ppc, inst);
	case OPCD31_X_PRTYW:
		return prtyw(ppc, inst);
	case OPCD31_X_EXTSW:
		return extsw(ppc, inst);
	case OPCD31_X_POPCNTD:
		return popcntd(ppc, inst);
	case OPCD31_X_CNTLZD:
		return cntlzd(ppc, inst);
	case OPCD31_X_SLW:
		return slw(ppc, inst);
	case OPCD31_X_SRW:
		return srw(ppc, inst);
	case OPCD31_X_SRAW:
		return sraw(ppc, inst);
	case OPCD31_X_SRAWI:
		return srawi(ppc, inst);
	case OPCD31_X_SLD:
		return sld(ppc, inst);
	case OPCD31_X_SRD:
		return srd(ppc, inst);
	case OPCD31_X_MFCR:
		return mfcr(ppc, inst);
	case OPCD31_X_CMP:
		return cmp(ppc, inst);
	case OPCD31_X_CMPL:
		return cmpl(ppc, inst);
	case OPCD31_X_STBX:
		return stbx(ppc, inst);
	case OPCD31_X_STBUX:
		return stbux(ppc, inst);
	case OPCD31_X_STHX:
		return sthx(ppc, inst);
	case OPCD31_X_STHUX:
		return sthux(ppc, inst);
	case OPCD31_X_STWX:
		return stwx(ppc, inst);
	case OPCD31_X_STWUX:
		return stwux(ppc, inst);
	case OPCD31_X_STDX:
		return stdx(ppc, inst);
	case OPCD31_X_STDUX:
		return stdux(ppc, inst);
	case OPCD31_X_LBZX:
		return lbzx(ppc, inst);
	case OPCD31_X_LBZUX:
		return lbzux(ppc, inst);
	case OPCD31_X_LHZX:
		return lhzx(ppc, inst);
	case OPCD31_X_LHZUX:
		return lhzux(ppc, inst);
	case OPCD31_X_LHAX:
		return lhax(ppc, inst);
	case OPCD31_X_LHAUX:
		return lhaux(ppc, inst);
	case OPCD31_X_LWZX:
		return lwzx(ppc, inst);
	case OPCD31_X_LWZUX:
		return lwzux(ppc, inst);
	case OPCD31_X_LWAX:
		return lwax(ppc, inst);
	case OPCD31_X_LWAUX:
		return lwaux(ppc, inst);
	case OPCD31_X_LDX:
		return ldx(ppc, inst);
	case OPCD31_X_LDUX:
		return ldux(ppc, inst);
	default:
		break;
	}
	switch(inst.xfxform.xo){
	case OPCD31_XFX_MTSPR:
		return mtspr(ppc, inst);
	case OPCD31_XFX_MFSPR:
		return mfspr(ppc, inst);
	default:
		break;
	}
	return RET_NOTIMPLEMENTED;
}

int opcd19(ppc64_t *ppc, inst_t inst)
{
	switch(inst.xlform.xo){
	case OPCD19_BCLR:
		return bclr(ppc, inst);
	case OPCD19_BCCTR:
		return bcctr(ppc, inst);
	case OPCD19_CRAND:
		return crand(ppc, inst);
	case OPCD19_CRNAND:
		return crnand(ppc, inst);
	case OPCD19_CROR:
		return cror(ppc, inst);
	case OPCD19_CRXOR:
		return crxor(ppc, inst);
	case OPCD19_CRNOR:
		return crnor(ppc, inst);
	case OPCD19_CREQV:
		return creqv(ppc, inst);
	case OPCD19_CRANDC:
		return crandc(ppc, inst);
	case OPCD19_CRORC:
		return crorc(ppc, inst);
	default:
		return RET_NOTIMPLEMENTED;
	}
}

int stb(ppc64_t *ppc, inst_t inst)
{
	unsigned long long b = inst.dform.ra == 0 ? 0 : ppc->regs.GPR[inst.dform.ra];
	unsigned long long ea = b + EXTS16(inst.dform.imm);
	unsigned char rs = ppc->regs.GPR[inst.dform.rt] & 0xFF;
	int err = WriteMemory1(&ppc->memory, ea, rs);
	if(err != 0){
		return RET_MEMORYERROR;
	}
	return RET_SUCCESS;
}
int stbx(ppc64_t *ppc, inst_t inst)
{
	unsigned long long b = inst.xform.ra == 0 ? 0 : ppc->regs.GPR[inst.xform.ra];
	unsigned long long ea = b + ppc->regs.GPR[inst.xform.rb];
	unsigned char rs = ppc->regs.GPR[inst.xform.rt] & 0xFF;
	int err = WriteMemory1(&ppc->memory, ea, rs);
	if(err != 0){
		return RET_MEMORYERROR;
	}
	return RET_SUCCESS;
}
int stbu(ppc64_t *ppc, inst_t inst)
{
	if(inst.dform.ra == 0) return RET_INVALIDINSTRUCTION;
	unsigned long long ea = ppc->regs.GPR[inst.dform.ra] + EXTS16(inst.dform.imm);
	unsigned char rs = ppc->regs.GPR[inst.dform.rt] & 0xFF;
	int err = WriteMemory1(&ppc->memory, ea, rs);
	if(err != 0){
		return RET_MEMORYERROR;
	}
	ppc->regs.GPR[inst.dform.ra] = ea;
	return RET_SUCCESS;
}
int stbux(ppc64_t *ppc, inst_t inst)
{
	if(inst.xform.ra == 0) return RET_INVALIDINSTRUCTION;
	unsigned long long ea = ppc->regs.GPR[inst.xform.ra] + ppc->regs.GPR[inst.xform.rb];
	int err = WriteMemory1(&ppc->memory, ea, ppc->regs.GPR[inst.xform.rt] & 0xFF);
	if(err != 0){
		return RET_MEMORYERROR;
	}
	ppc->regs.GPR[inst.xform.ra] = ea;
	return RET_SUCCESS;
}
int sth(ppc64_t *ppc, inst_t inst)
{
	unsigned long long b = inst.dform.ra == 0 ? 0 : ppc->regs.GPR[inst.dform.ra];
	unsigned long long ea = b + EXTS16(inst.dform.imm);
	unsigned short rs = ppc->regs.GPR[inst.dform.rt] & 0xFFFF;
	int err = WriteMemory2(&ppc->memory, ea, rs);
	if(err != 0){
		return RET_MEMORYERROR;
	}
	return RET_SUCCESS;
}
int sthx(ppc64_t *ppc, inst_t inst)
{
	unsigned long long b = inst.xform.ra == 0 ? 0 : ppc->regs.GPR[inst.xform.ra];
	unsigned long long ea = b + ppc->regs.GPR[inst.xform.rb];
	unsigned short rs = ppc->regs.GPR[inst.xform.rt] & 0xFFFF;
	int err = WriteMemory2(&ppc->memory, ea, rs);
	if(err != 0){
		return RET_MEMORYERROR;
	}
	return RET_SUCCESS;
}
int sthu(ppc64_t *ppc, inst_t inst)
{
	if(inst.dform.ra == 0) return RET_INVALIDINSTRUCTION;
	unsigned long long ea = ppc->regs.GPR[inst.dform.ra] + EXTS16(inst.dform.imm);
	unsigned short rs = ppc->regs.GPR[inst.dform.rt] & 0xFFFF;
	int err = WriteMemory2(&ppc->memory, ea, rs);
	if(err != 0){
		return RET_MEMORYERROR;
	}
	ppc->regs.GPR[inst.dform.ra] = ea;
	return RET_SUCCESS;
}
int sthux(ppc64_t *ppc, inst_t inst)
{
	if(inst.xform.ra == 0) return RET_INVALIDINSTRUCTION;
	unsigned long long ea = ppc->regs.GPR[inst.xform.ra] + ppc->regs.GPR[inst.xform.rb];
	int err = WriteMemory2(&ppc->memory, ea, ppc->regs.GPR[inst.xform.rt] & 0xFFFF);
	if(err != 0){
		return RET_MEMORYERROR;
	}
	ppc->regs.GPR[inst.xform.ra] = ea;
	return RET_SUCCESS;
}
int stw(ppc64_t *ppc, inst_t inst)
{
	unsigned long long b = inst.dform.ra == 0 ? 0 : ppc->regs.GPR[inst.dform.ra];
	unsigned long long ea = b + EXTS16(inst.dform.imm);
	unsigned int rs = ppc->regs.GPR[inst.dform.rt] & 0xFFFFFFFF;
	int err = WriteMemory4(&ppc->memory, ea, rs);
	if(err != 0){
		return RET_MEMORYERROR;
	}
	return RET_SUCCESS;
}
int stwx(ppc64_t *ppc, inst_t inst)
{
	unsigned long long b = inst.xform.ra == 0 ? 0 : ppc->regs.GPR[inst.xform.ra];
	unsigned long long ea = b + ppc->regs.GPR[inst.xform.rb];
	unsigned int rs = ppc->regs.GPR[inst.xform.rt] & 0xFFFFFFFF;
	int err = WriteMemory4(&ppc->memory, ea, rs);
	if(err != 0){
		return RET_MEMORYERROR;
	}
	return RET_SUCCESS;
}
int stwu(ppc64_t *ppc, inst_t inst)
{
	if(inst.dform.ra == 0) return RET_INVALIDINSTRUCTION;
	unsigned long long ea = ppc->regs.GPR[inst.dform.ra] + EXTS16(inst.dform.imm);
	unsigned int rs = ppc->regs.GPR[inst.dform.rt] & 0xFFFFFFFF;
	int err = WriteMemory4(&ppc->memory, ea, rs);
	if(err != 0){
		return RET_MEMORYERROR;
	}
	ppc->regs.GPR[inst.dform.ra] = ea;
	return RET_SUCCESS;
}
int stwux(ppc64_t *ppc, inst_t inst)
{
	if(inst.xform.ra == 0) return RET_INVALIDINSTRUCTION;
	unsigned long long ea = ppc->regs.GPR[inst.xform.ra] + ppc->regs.GPR[inst.xform.rb];
	int err = WriteMemory4(&ppc->memory, ea, ppc->regs.GPR[inst.xform.rt] & 0xFFFFFFFF);
	if(err != 0){
		return RET_MEMORYERROR;
	}
	ppc->regs.GPR[inst.xform.ra] = ea;
	return RET_SUCCESS;
}
int std(ppc64_t *ppc, inst_t inst)
{
	unsigned long long b = inst.dsform.ra == 0 ? 0 : ppc->regs.GPR[inst.dsform.ra];
	unsigned long long ea = b + EXTS16(inst.dsform.ds << 2);
	unsigned long long rs = ppc->regs.GPR[inst.dsform.rt];
	int err = WriteMemory8(&ppc->memory, ea, rs);
	if(err != 0){
		return RET_MEMORYERROR;
	}
	return RET_SUCCESS;
}
int stdx(ppc64_t *ppc, inst_t inst)
{
	unsigned long long b = inst.xform.ra == 0 ? 0 : ppc->regs.GPR[inst.xform.ra];
	unsigned long long ea = b + ppc->regs.GPR[inst.xform.rb];
	unsigned long long rs = ppc->regs.GPR[inst.xform.rt];
	int err = WriteMemory8(&ppc->memory, ea, rs);
	if(err != 0){
		return RET_MEMORYERROR;
	}
	return RET_SUCCESS;
}
int stdu(ppc64_t *ppc, inst_t inst)
{
	if(inst.dsform.ra == 0) return RET_INVALIDINSTRUCTION;
	unsigned long long ea = ppc->regs.GPR[inst.dsform.ra] + EXTS16(inst.dsform.ds << 2);
	unsigned long long rs = ppc->regs.GPR[inst.dsform.rt];
	int err = WriteMemory8(&ppc->memory, ea, rs);
	if(err != 0){
		return RET_MEMORYERROR;
	}
	ppc->regs.GPR[inst.dsform.ra] = ea;
	return RET_SUCCESS;
}
int stdux(ppc64_t *ppc, inst_t inst)
{
	if(inst.xform.ra == 0) return RET_INVALIDINSTRUCTION;
	unsigned long long ea = ppc->regs.GPR[inst.xform.ra] + ppc->regs.GPR[inst.xform.rb];
	int err = WriteMemory8(&ppc->memory, ea, ppc->regs.GPR[inst.xform.rt]);
	if(err != 0){
		return RET_MEMORYERROR;
	}
	ppc->regs.GPR[inst.xform.ra] = ea;
	return RET_SUCCESS;
}
int lbz(ppc64_t *ppc, inst_t inst)
{
	unsigned long long b = inst.dform.ra == 0 ? 0 : ppc->regs.GPR[inst.dform.ra];
	unsigned long long ea = b + EXTS16(inst.dform.imm);
	unsigned char rt;
	int err = ReadMemory1(&ppc->memory, ea, &rt);
	if(err != 0){
		return RET_MEMORYERROR;
	}
	ppc->regs.GPR[inst.dform.rt] = rt;
	return RET_SUCCESS;
}
int lbzx(ppc64_t *ppc, inst_t inst)
{
	unsigned long long b = inst.xform.ra == 0 ? 0 : ppc->regs.GPR[inst.xform.ra];
	unsigned long long ea = b + ppc->regs.GPR[inst.xform.rb];
	unsigned char rt;
	int err = ReadMemory1(&ppc->memory, ea, &rt);
	if(err != 0){
		return RET_MEMORYERROR;
	}
	ppc->regs.GPR[inst.xform.rt] = rt;
	return RET_SUCCESS;
}
int lbzu(ppc64_t *ppc, inst_t inst)
{
	if(inst.dform.ra == 0 || inst.dform.ra == inst.dform.rt) return RET_INVALIDINSTRUCTION;
	unsigned long long ea = ppc->regs.GPR[inst.dform.ra] + EXTS16(inst.dform.imm);
	unsigned char rt;
	int err = ReadMemory1(&ppc->memory, ea, &rt);
	if(err != 0){
		return RET_MEMORYERROR;
	}
	ppc->regs.GPR[inst.dform.rt] = rt;
	ppc->regs.GPR[inst.dform.ra] = ea;
	return RET_SUCCESS;
}
int lbzux(ppc64_t *ppc, inst_t inst)
{
	if(inst.xform.ra == 0 || inst.xform.ra == inst.xform.rt) return RET_INVALIDINSTRUCTION;
	unsigned long long ea = ppc->regs.GPR[inst.xform.ra] + ppc->regs.GPR[inst.xform.rb];
	unsigned char rt;
	int err = ReadMemory1(&ppc->memory, ea, &rt);
	if(err != 0){
		return RET_MEMORYERROR;
	}
	ppc->regs.GPR[inst.xform.rt] = rt;
	ppc->regs.GPR[inst.xform.ra] = ea;
	return RET_SUCCESS;
}
int lhz(ppc64_t *ppc, inst_t inst)
{
	unsigned long long b = inst.dform.ra == 0 ? 0 : ppc->regs.GPR[inst.dform.ra];
	unsigned long long ea = b + EXTS16(inst.dform.imm);
	unsigned short rt;
	int err = ReadMemory2(&ppc->memory, ea, &rt);
	if(err != 0){
		return RET_MEMORYERROR;
	}
	ppc->regs.GPR[inst.dform.rt] = rt;
	return RET_SUCCESS;
}
int lhzx(ppc64_t *ppc, inst_t inst)
{
	unsigned long long b = inst.xform.ra == 0 ? 0 : ppc->regs.GPR[inst.xform.ra];
	unsigned long long ea = b + ppc->regs.GPR[inst.xform.rb];
	unsigned short rt;
	int err = ReadMemory2(&ppc->memory, ea, &rt);
	if(err != 0){
		return RET_MEMORYERROR;
	}
	ppc->regs.GPR[inst.xform.rt] = rt;
	return RET_SUCCESS;
}
int lhzu(ppc64_t *ppc, inst_t inst)
{
	if(inst.dform.ra == 0 || inst.dform.ra == inst.dform.rt) return RET_INVALIDINSTRUCTION;
	unsigned long long ea = ppc->regs.GPR[inst.dform.ra] + EXTS16(inst.dform.imm);
	unsigned short rt;
	int err = ReadMemory2(&ppc->memory, ea, &rt);
	if(err != 0){
		return RET_MEMORYERROR;
	}
	ppc->regs.GPR[inst.dform.rt] = rt;
	ppc->regs.GPR[inst.dform.ra] = ea;
	return RET_SUCCESS;
}
int lhzux(ppc64_t *ppc, inst_t inst)
{
	if(inst.xform.ra == 0 || inst.xform.ra == inst.xform.rt) return RET_INVALIDINSTRUCTION;
	unsigned long long ea = ppc->regs.GPR[inst.xform.ra] + ppc->regs.GPR[inst.xform.rb];
	unsigned short rt;
	int err = ReadMemory2(&ppc->memory, ea, &rt);
	if(err != 0){
		return RET_MEMORYERROR;
	}
	ppc->regs.GPR[inst.xform.rt] = rt;
	ppc->regs.GPR[inst.xform.ra] = ea;
	return RET_SUCCESS;
}
int lha(ppc64_t *ppc, inst_t inst)
{
	unsigned long long b = inst.dform.ra == 0 ? 0 : ppc->regs.GPR[inst.dform.ra];
	unsigned long long ea = b + EXTS16(inst.dform.imm);
	unsigned short rt;
	int err = ReadMemory2(&ppc->memory, ea, &rt);
	if(err != 0){
		return RET_MEMORYERROR;
	}
	ppc->regs.GPR[inst.dform.rt] = EXTS16(rt);
	return RET_SUCCESS;
}
int lhax(ppc64_t *ppc, inst_t inst)
{
	unsigned long long b = inst.xform.ra == 0 ? 0 : ppc->regs.GPR[inst.xform.ra];
	unsigned long long ea = b + ppc->regs.GPR[inst.xform.rb];
	unsigned short rt;
	int err = ReadMemory2(&ppc->memory, ea, &rt);
	if(err != 0){
		return RET_MEMORYERROR;
	}
	ppc->regs.GPR[inst.xform.rt] = EXTS16(rt);
	return RET_SUCCESS;
}
int lhau(ppc64_t *ppc, inst_t inst)
{
	if(inst.dform.ra == 0 || inst.dform.ra == inst.dform.rt) return RET_INVALIDINSTRUCTION;
	unsigned long long ea = ppc->regs.GPR[inst.dform.ra] + EXTS16(inst.dform.imm);
	unsigned short rt;
	int err = ReadMemory2(&ppc->memory, ea, &rt);
	if(err != 0){
		return RET_MEMORYERROR;
	}
	ppc->regs.GPR[inst.dform.rt] = EXTS16(rt);
	ppc->regs.GPR[inst.dform.ra] = ea;
	return RET_SUCCESS;
}
int lhaux(ppc64_t *ppc, inst_t inst)
{
	if(inst.xform.ra == 0 || inst.xform.ra == inst.xform.rt) return RET_INVALIDINSTRUCTION;
	unsigned long long ea = ppc->regs.GPR[inst.xform.ra] + ppc->regs.GPR[inst.xform.rb];
	unsigned short rt;
	int err = ReadMemory2(&ppc->memory, ea, &rt);
	if(err != 0){
		return RET_MEMORYERROR;
	}
	ppc->regs.GPR[inst.xform.rt] = EXTS16(rt);
	ppc->regs.GPR[inst.xform.ra] = ea;
	return RET_SUCCESS;
}
int lwz(ppc64_t *ppc, inst_t inst)
{
	unsigned long long b = inst.dform.ra == 0 ? 0 : ppc->regs.GPR[inst.dform.ra];
	unsigned long long ea = b + EXTS16(inst.dform.imm);
	unsigned int rt;
	int err = ReadMemory4(&ppc->memory, ea, &rt);
	if(err != 0){
		return RET_MEMORYERROR;
	}
	ppc->regs.GPR[inst.dform.rt] = rt;
	return RET_SUCCESS;
}
int lwzx(ppc64_t *ppc, inst_t inst)
{
	unsigned long long b = inst.xform.ra == 0 ? 0 : ppc->regs.GPR[inst.xform.ra];
	unsigned long long ea = b + ppc->regs.GPR[inst.xform.rb];
	unsigned int rt;
	int err = ReadMemory4(&ppc->memory, ea, &rt);
	if(err != 0){
		return RET_MEMORYERROR;
	}
	ppc->regs.GPR[inst.xform.rt] = rt;
	return RET_SUCCESS;
}
int lwzu(ppc64_t *ppc, inst_t inst)
{
	if(inst.dform.ra == 0 || inst.dform.ra == inst.dform.rt) return RET_INVALIDINSTRUCTION;
	unsigned long long ea = ppc->regs.GPR[inst.dform.ra] + EXTS16(inst.dform.imm);
	unsigned int rt;
	int err = ReadMemory4(&ppc->memory, ea, &rt);
	if(err != 0){
		return RET_MEMORYERROR;
	}
	ppc->regs.GPR[inst.dform.rt] = rt;
	ppc->regs.GPR[inst.dform.ra] = ea;
	return RET_SUCCESS;
}
int lwzux(ppc64_t *ppc, inst_t inst)
{
	if(inst.xform.ra == 0 || inst.xform.ra == inst.xform.rt) return RET_INVALIDINSTRUCTION;
	unsigned long long ea = ppc->regs.GPR[inst.xform.ra] + ppc->regs.GPR[inst.xform.rb];
	unsigned int rt;
	int err = ReadMemory4(&ppc->memory, ea, &rt);
	if(err != 0){
		return RET_MEMORYERROR;
	}
	ppc->regs.GPR[inst.xform.rt] = rt;
	ppc->regs.GPR[inst.xform.ra] = ea;
	return RET_SUCCESS;
}
int lwa(ppc64_t *ppc, inst_t inst)
{
	unsigned long long b = inst.dsform.ra == 0 ? 0 : ppc->regs.GPR[inst.dsform.ra];
	unsigned long long ea = b + EXTS16(inst.dsform.ds << 2);
	unsigned int rt;
	int err = ReadMemory4(&ppc->memory, ea, &rt);
	if(err != 0){
		return RET_MEMORYERROR;
	}
	ppc->regs.GPR[inst.dsform.rt] = EXTS32(rt);
	return RET_SUCCESS;
}
int lwax(ppc64_t *ppc, inst_t inst)
{
	unsigned long long b = inst.xform.ra == 0 ? 0 : ppc->regs.GPR[inst.xform.ra];
	unsigned long long ea = b + ppc->regs.GPR[inst.xform.rb];
	unsigned int rt;
	int err = ReadMemory4(&ppc->memory, ea, &rt);
	if(err != 0){
		return RET_MEMORYERROR;
	}
	ppc->regs.GPR[inst.dsform.rt] = EXTS32(rt);
	return RET_SUCCESS;
}
int lwaux(ppc64_t *ppc, inst_t inst)
{
	if(inst.xform.ra == 0 || inst.xform.ra == inst.xform.rt) return RET_INVALIDINSTRUCTION;
	unsigned long long ea = ppc->regs.GPR[inst.xform.ra] + ppc->regs.GPR[inst.xform.rb];
	unsigned int rt;
	int err = ReadMemory4(&ppc->memory, ea, &rt);
	if(err != 0){
		return RET_MEMORYERROR;
	}
	ppc->regs.GPR[inst.xform.rt] = EXTS32(rt);
	ppc->regs.GPR[inst.xform.ra] = ea;
	return RET_SUCCESS;
}
int ld(ppc64_t *ppc, inst_t inst)
{
	unsigned long long b = inst.dsform.ra == 0 ? 0 : ppc->regs.GPR[inst.dsform.ra];
	unsigned long long ea = b + EXTS16(inst.dsform.ds << 2);
	unsigned long long rt;
	int err = ReadMemory8(&ppc->memory, ea, &rt);
	if(err != 0){
		return RET_MEMORYERROR;
	}
	ppc->regs.GPR[inst.dsform.rt] = rt;
	return RET_SUCCESS;
}
int ldx(ppc64_t *ppc, inst_t inst)
{
	unsigned long long b = inst.xform.ra == 0 ? 0 : ppc->regs.GPR[inst.xform.ra];
	unsigned long long ea = b + ppc->regs.GPR[inst.xform.rb];
	unsigned long long rt;
	int err = ReadMemory8(&ppc->memory, ea, &rt);
	if(err != 0){
		return RET_MEMORYERROR;
	}
	ppc->regs.GPR[inst.xform.rt] = rt;
	return RET_SUCCESS;
}
int ldu(ppc64_t *ppc, inst_t inst)
{
	if(inst.dsform.ra == 0 || inst.dsform.ra == inst.dsform.rt) return RET_INVALIDINSTRUCTION;
	unsigned long long ea = ppc->regs.GPR[inst.dsform.ra] + EXTS16(inst.dsform.ds << 2);
	unsigned long long rt;
	int err = ReadMemory8(&ppc->memory, ea, &rt);
	if(err != 0){
		return RET_MEMORYERROR;
	}
	ppc->regs.GPR[inst.dsform.rt] = rt;
	ppc->regs.GPR[inst.dsform.ra] = ea;
	return RET_SUCCESS;
}
int ldux(ppc64_t *ppc, inst_t inst)
{
	if(inst.xform.ra == 0 || inst.xform.ra == inst.xform.rt) return RET_INVALIDINSTRUCTION;
	unsigned long long ea = ppc->regs.GPR[inst.xform.ra] + ppc->regs.GPR[inst.xform.rb];
	unsigned long long rt;
	int err = ReadMemory8(&ppc->memory, ea, &rt);
	if(err != 0){
		return RET_MEMORYERROR;
	}
	ppc->regs.GPR[inst.xform.rt] = rt;
	ppc->regs.GPR[inst.xform.ra] = ea;
	return RET_SUCCESS;
}

static inline unsigned long long mask(unsigned int x, unsigned int y)
{
	unsigned long long m1 = x == 0 ? 0xFFFFFFFFFFFFFFFF : (1ull << (64 - x)) - 1;
	unsigned long long m2 = (1ull << (63 - y)) - 1;
	if(x <= y){
		return m1 & ~m2;
	}
	else{
		return m1 | ~m2;
	}
}
//上位32bitは0になる?
//http://www.super-computing.org/~ysd/yuke/ppc64.html
int rlwinm(ppc64_t *ppc, inst_t inst)
{
	unsigned int n = inst.mform.rb;
	unsigned long long rs = ppc->regs.GPR[inst.mform.rs];
	unsigned long long r = rotl32(rs & 0xFFFFFFFF, n);
	unsigned long long m = mask(inst.mform.mb+32, inst.mform.me+32);
	unsigned long long result = r & m;
	if(inst.mform.rc){
		ppc->regs.CR.field.cr0 = condition(ppc, result, 0);
	}
	ppc->regs.GPR[inst.mform.ra] = result;
	return RET_SUCCESS;
}
int rlwnm(ppc64_t *ppc, inst_t inst)
{
	unsigned int n = ppc->regs.GPR[inst.mform.rb] & 0x1F;
	unsigned long long rs = ppc->regs.GPR[inst.mform.rs];
	unsigned long long r = rotl32(rs & 0xFFFFFFFF, n);
	unsigned long long m = mask(inst.mform.mb+32, inst.mform.me+32);
	unsigned long long result = r & m;
	if(inst.mform.rc){
		ppc->regs.CR.field.cr0 = condition(ppc, result, 0);
	}
	ppc->regs.GPR[inst.mform.ra] = result;
	return RET_SUCCESS;
}
int rlwimi(ppc64_t *ppc, inst_t inst)
{
	unsigned int n = inst.mform.rb;
	unsigned long long rs = ppc->regs.GPR[inst.mform.rs];
	unsigned long long r = rotl32(rs & 0xFFFFFFFF, n);
	unsigned long long m = mask(inst.mform.mb+32, inst.mform.me+32);
	unsigned long long ra = ppc->regs.GPR[inst.mform.ra];
	unsigned long long result = (r & m) | (ra & ~m);
	if(inst.mform.rc){
		ppc->regs.CR.field.cr0 = condition(ppc, result, 0);
	}
	ppc->regs.GPR[inst.mform.ra] = result;
	return RET_SUCCESS;
}
int rldicl(ppc64_t *ppc, inst_t inst)
{
	unsigned int n = inst.mdform.sh1 | (inst.mdform.sh2 << 5);
	unsigned int b = (inst.mdform.mb >> 1) | ((inst.mdform.mb & 1) << 5);
	unsigned long long m = b == 0 ? ~0ull : ((1ull << (64-b)) - 1);
	unsigned long long rs = ppc->regs.GPR[inst.mdform.rs];
	unsigned long long r = rotl64(rs, n);
	unsigned long long result = r & m;
	ppc->regs.GPR[inst.mdform.ra] = result;
	if(inst.mdform.rc){
		ppc->regs.CR.field.cr0 = condition(ppc, result, 0);
	}
	return RET_SUCCESS;
}
int rldicr(ppc64_t *ppc, inst_t inst)
{
	unsigned int n = inst.mdform.sh1 | (inst.mdform.sh2 << 5);
	unsigned int e = (inst.mdform.mb >> 1) | ((inst.mdform.mb & 1) << 5);
	unsigned long long m = ~((1ull << (63 - e)) - 1);
	unsigned long long rs = ppc->regs.GPR[inst.mdform.rs];
	unsigned long long r = rotl64(rs, n);
	unsigned long long result = r & m;
	ppc->regs.GPR[inst.mdform.ra] = result;
	if(inst.mdform.rc){
		ppc->regs.CR.field.cr0 = condition(ppc, result, 0);
	}
	return RET_SUCCESS;
}
int rldic(ppc64_t *ppc, inst_t inst)
{
	unsigned int n = inst.mdform.sh1 | (inst.mdform.sh2 << 5);
	unsigned int b = (inst.mdform.mb >> 1) | ((inst.mdform.mb & 1) << 5);
	unsigned long long m = mask(b, 63 - n);
	unsigned long long rs = ppc->regs.GPR[inst.mdform.rs];
	unsigned long long r = rotl64(rs, n);
	unsigned long long result = r & m;
	ppc->regs.GPR[inst.mdform.ra] = result;
	if(inst.mdform.rc){
		ppc->regs.CR.field.cr0 = condition(ppc, result, 0);
	}
	return RET_SUCCESS;
}
int rldcl(ppc64_t *ppc, inst_t inst)
{
	unsigned int n = ppc->regs.GPR[inst.mdsform.rb] & 0x3F;
	unsigned long long r = rotl64(ppc->regs.GPR[inst.mdsform.rs], n);
	unsigned int b = (inst.mdsform.mb >> 1) | ((inst.mdsform.mb & 1) << 5);
	unsigned long long m = mask(b, 63);
	unsigned long long result = r & m;
	ppc->regs.GPR[inst.mdsform.ra] = result;
	if(inst.mdsform.rc){
		ppc->regs.CR.field.cr0 = condition(ppc, result, 0);
	}
	return RET_SUCCESS;
}
int rldcr(ppc64_t *ppc, inst_t inst)
{
	unsigned int n = ppc->regs.GPR[inst.mdsform.rb] & 0x3F;
	unsigned long long r = rotl64(ppc->regs.GPR[inst.mdsform.rs], n);
	unsigned int e = (inst.mdsform.mb >> 1) | ((inst.mdsform.mb & 1) << 5);
	unsigned long long m = mask(0, e);
	unsigned long long result = r & m;
	ppc->regs.GPR[inst.mdsform.ra] = result;
	if(inst.mdsform.rc){
		ppc->regs.CR.field.cr0 = condition(ppc, result, 0);
	}
	return RET_SUCCESS;
}
int rldimi(ppc64_t *ppc, inst_t inst)
{
	unsigned int n = inst.mdform.sh1 | (inst.mdform.sh2 << 5);
	unsigned long long rs = ppc->regs.GPR[inst.mdform.rs];
	unsigned long long r = rotl64(rs, n);
	unsigned int b = (inst.mdform.mb >> 1) | ((inst.mdform.mb & 1) << 5);
	unsigned long long m = mask(b, 63 - n);
	unsigned long long ra = ppc->regs.GPR[inst.mdform.ra];
	unsigned long long result = (r & m) | (ra & ~m);
	ppc->regs.GPR[inst.mdform.ra] = result;
	if(inst.mdform.rc){
		ppc->regs.CR.field.cr0 = condition(ppc, result, 0);
	}
	return RET_SUCCESS;
}

int opcd30(ppc64_t *ppc, inst_t inst)
{
	switch(inst.mdform.xo){
	case OPCD30_MD_RLDICR:
		return rldicr(ppc, inst);
	case OPCD30_MD_RLDICL:
		return rldicl(ppc, inst);
	case OPCD30_MD_RLDIC:
		return rldic(ppc, inst);
	case OPCD30_MD_RLDIMI:
		return rldimi(ppc, inst);
	default:
		break;
	}
	switch(inst.mdsform.xo){
	case OPCD30_MDS_RLDCL:
		return rldcl(ppc, inst);
	case OPCD30_MDS_RLDCR:
		return rldcr(ppc, inst);
	default:
		break;
	}
	return RET_NOTIMPLEMENTED;
}

int sc(ppc64_t *ppc, inst_t inst)
{
	return RET_NOTIMPLEMENTED;
}

int mulli(ppc64_t *ppc, inst_t inst)
{
	return RET_NOTIMPLEMENTED;
}
int mulhw(ppc64_t *ppc, inst_t inst)
{
	return RET_NOTIMPLEMENTED;
}
int mullw(ppc64_t *ppc, inst_t inst)
{
	return RET_NOTIMPLEMENTED;
}
int mulhwu(ppc64_t *ppc, inst_t inst)
{
	return RET_NOTIMPLEMENTED;
}
int divw(ppc64_t *ppc, inst_t inst)
{
	return RET_NOTIMPLEMENTED;
}
int divwu(ppc64_t *ppc, inst_t inst)
{
	return RET_NOTIMPLEMENTED;
}
int divwe(ppc64_t *ppc, inst_t inst)
{
	return RET_NOTIMPLEMENTED;
}
int divweu(ppc64_t *ppc, inst_t inst)
{
	return RET_NOTIMPLEMENTED;
}
int mulld(ppc64_t *ppc, inst_t inst)
{
	return RET_NOTIMPLEMENTED;
}
int mulhd(ppc64_t *ppc, inst_t inst)
{
	return RET_NOTIMPLEMENTED;
}
int mulhdu(ppc64_t *ppc, inst_t inst)
{
	return RET_NOTIMPLEMENTED;
}
int divd(ppc64_t *ppc, inst_t inst)
{
	return RET_NOTIMPLEMENTED;
}
int divdu(ppc64_t *ppc, inst_t inst)
{
	return RET_NOTIMPLEMENTED;
}
int divde(ppc64_t *ppc, inst_t inst)
{
	return RET_NOTIMPLEMENTED;
}
int divdeu(ppc64_t *ppc, inst_t inst)
{
	return RET_NOTIMPLEMENTED;
}

int prtyd(ppc64_t *ppc, inst_t inst)
{
	unsigned long long rs = ppc->regs.GPR[inst.xform.rt];
	unsigned long long ra = rs ^ (rs >> 32);
	ra = ra ^ (ra >> 16);
	ra = ra ^ (ra >> 8);
	ra &= 1;
	ppc->regs.GPR[inst.xform.ra] = ra;
	return RET_SUCCESS;
}
int prtyw(ppc64_t *ppc, inst_t inst)
{
	unsigned long long rs = ppc->regs.GPR[inst.xform.rt];
	unsigned long long ra = rs ^ (rs >> 8);
	ra = ra ^ (ra >> 16);
	ra &= 0x0000000100000001;
	return RET_NOTIMPLEMENTED;
}
int extsw(ppc64_t *ppc, inst_t inst)
{
	unsigned long long rs = ppc->regs.GPR[inst.xform.rt];
	unsigned long long result = EXTS32(rs & 0xFFFFFFFF);
	ppc->regs.GPR[inst.xform.ra] = result;
	if(inst.xform.rc){
		ppc->regs.CR.field.cr0 = condition(ppc, result, 0);
	}
	return RET_SUCCESS;
}
int popcntd(ppc64_t *ppc, inst_t inst)
{
	unsigned long long rs = ppc->regs.GPR[inst.xform.rt];
	unsigned long long result;
	result = (rs & MASK1) + ((rs >> 1) & MASK1);
	result = (result & MASK2) + ((result >> 2) & MASK2);
	result = (result & MASK4) + ((result >> 4) & MASK4);
	result = (result & MASK8) + ((result >> 8) & MASK8);
	result = (result & MASK16) + ((result >> 16) & MASK16);
	result = (result & MASK32) + ((result >> 32) & MASK32);
	ppc->regs.GPR[inst.xform.ra] = result;
	return RET_SUCCESS;
}
int cntlzd(ppc64_t *ppc, inst_t inst)
{
	unsigned long long rs = ppc->regs.GPR[inst.xform.rt];
	long long result = 63;
	for(;result>=0;result--){
		if((rs >> result) & 1) break;
	}
	result = 63 - result;
	ppc->regs.GPR[inst.xform.ra] = result;
	if(inst.xform.rc){
		ppc->regs.CR.field.cr0 = condition(ppc, result, 0);
	}
	return RET_SUCCESS;
}

int sld(ppc64_t *ppc, inst_t inst)
{
	unsigned long long rs = ppc->regs.GPR[inst.xform.rt];
	unsigned long long rb = ppc->regs.GPR[inst.xform.rb];
	unsigned int n = rb & 0x3F;
	unsigned long long result = rs << n;
	if((rb >> 6) & 1){
		result = 0;
	}
	ppc->regs.GPR[inst.xform.ra] = result;
	if(inst.xform.rc){
		ppc->regs.CR.field.cr0 = condition(ppc, result, 0);
	}
	return RET_SUCCESS;
}

int srd(ppc64_t *ppc, inst_t inst)
{
	unsigned long long rs = ppc->regs.GPR[inst.xform.rt];
	unsigned long long rb = ppc->regs.GPR[inst.xform.rb];
	unsigned int n = rb & 0x3F;
	unsigned long long result = rs >> n;
	if((rb >> 6) & 1){
		result = 0;
	}
	ppc->regs.GPR[inst.xform.ra] = result;
	if(inst.xform.rc){
		ppc->regs.CR.field.cr0 = condition(ppc, result, 0);
	}
	return RET_SUCCESS;
}
