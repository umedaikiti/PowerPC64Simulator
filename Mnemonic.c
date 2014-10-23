#include "Mnemonic.h"
#include "Instructions.h"
#include <stdio.h>

#define EXTS16(x) (((long long)(short)(x) << 48) >> 48)

static inline int GetMnemonicDFormRTRASI(char *buf, unsigned int n, inst_t inst, const char *str)
{
	return snprintf(buf, n, "%s\t%u, %u, %lld", str, inst.dform.rt, inst.dform.ra, EXTS16(inst.dform.imm));
}

static inline int GetMnemonicDFormRARSUI(char *buf, unsigned int n, inst_t inst, const char *str)
{
	return snprintf(buf, n, "%s\t%u, %u, 0x%x", str, inst.dform.ra, inst.dform.rt, inst.dform.imm);
}

static inline int GetMnemonicDFormLoadStore(char *buf, unsigned int n, inst_t inst, const char *str)
{
	return snprintf(buf, n, "%s\t%u, %lld(%u)", str, inst.dform.rt, EXTS16(inst.dform.imm), inst.dform.ra);
}

static inline int GetMnemonicXLFormBTBABB(char *buf, unsigned int n, inst_t inst, const char *str)
{
	return snprintf(buf, n, "%s\t%u, %u, %u", str, inst.xlform.bt, inst.xlform.ba, inst.xlform.bb);
}

static inline int GetMnemonicXOFormRTRARBOERc(char *buf, unsigned int n, inst_t inst, const char *str)
{
	const char *suf1 = inst.xoform.oe ? "o" : "";
	const char *suf2 = inst.xoform.rc ? "." : "";
	return snprintf(buf, n, "%s%s%s\t%u, %u, %u", str, suf1, suf2, inst.xoform.rt, inst.xoform.ra, inst.xoform.rb);
}

static inline int GetMnemonicXOFormRTRAOERc(char *buf, unsigned int n, inst_t inst, const char *str)
{
	const char *suf1 = inst.xoform.oe ? "o" : "";
	const char *suf2 = inst.xoform.rc ? "." : "";
	return snprintf(buf, n, "%s%s%s\t%u, %u", str, suf1, suf2, inst.xoform.rt, inst.xoform.ra);
}

static inline int GetMnemonicXFormRARSRBRc(char *buf, unsigned int n, inst_t inst, const char *str)
{
	const char *suf = inst.xform.rc ? "." : "";
	return snprintf(buf, n, "%s%s\t%u, %u, %u", str, suf, inst.xform.ra, inst.xform.rt, inst.xform.rb);
}

static inline int GetMnemonicXFormRARSRc(char *buf, unsigned int n, inst_t inst, const char *str)
{
	const char *suf = inst.xform.rc ? "." : "";
	return snprintf(buf, n, "%s%s\t%u, %u", str, suf, inst.xform.ra, inst.xform.rt);
}

static inline int GetMnemonicXFormRARS(char *buf, unsigned int n, inst_t inst, const char *str)
{
	return snprintf(buf, n, "%s\t%u, %u", str, inst.xform.ra, inst.xform.rt);
}

//bufにニーモニックを返す
//n: bufのサイズ
int GetMnemonic(char *buf, unsigned int n, inst_t inst)
{
	switch(inst.common.opcd){
	case SUBFIC:
		return GetMnemonicDFormRTRASI(buf, n, inst, "subfic");
	case ADDIC:
		return GetMnemonicDFormRTRASI(buf, n, inst, "addic");
	case ADDIC_cr0:
		return GetMnemonicDFormRTRASI(buf, n, inst, "addic.");
	case ADDI:
		if(inst.dform.ra == 0){
			return snprintf(buf, n, "li\t%u, %lld", inst.dform.rt, EXTS16(inst.dform.imm));
		}
		return GetMnemonicDFormRTRASI(buf, n, inst, "addi");
	case ADDIS:
		if(inst.dform.ra == 0){
			return snprintf(buf, n, "lis\t%u, %lld", inst.dform.rt, EXTS16(inst.dform.imm));
		}
		return GetMnemonicDFormRTRASI(buf, n, inst, "addis");
	case ANDI:
		return GetMnemonicDFormRARSUI(buf, n, inst, "andi.");
	case ORI:
		if(inst.dform.rt == 0 && inst.dform.ra == 0 && inst.dform.imm == 0){
			return snprintf(buf, n, "nop");
		}
		return GetMnemonicDFormRARSUI(buf, n, inst, "ori");
	case ANDIS:
		return GetMnemonicDFormRARSUI(buf, n, inst, "andis.");
	case ORIS:
		return GetMnemonicDFormRARSUI(buf, n, inst, "oris");
	case XORI:
		if(inst.dform.rt == 0 && inst.dform.ra == 0 && inst.dform.imm == 0){
			return snprintf(buf, n, "xnop");
		}
		return GetMnemonicDFormRARSUI(buf, n, inst, "xori");
	case XORIS:
		return GetMnemonicDFormRARSUI(buf, n, inst, "xoris");
	case LBZ:
		return GetMnemonicDFormLoadStore(buf, n, inst, "lbz");
	case LBZU:
		return GetMnemonicDFormLoadStore(buf, n, inst, "lbzu");
	case LHZ:
		return GetMnemonicDFormLoadStore(buf, n, inst, "lhz");
	case LHZU:
		return GetMnemonicDFormLoadStore(buf, n, inst, "lhzu");
	case LHA:
		return GetMnemonicDFormLoadStore(buf, n, inst, "lha");
	case LHAU:
		return GetMnemonicDFormLoadStore(buf, n, inst, "lhau");
	case LWZ:
		return GetMnemonicDFormLoadStore(buf, n, inst, "lwz");
	case LWZU:
		return GetMnemonicDFormLoadStore(buf, n, inst, "lwzu");
	case STB:
		return GetMnemonicDFormLoadStore(buf, n, inst, "stb");
	case STBU:
		return GetMnemonicDFormLoadStore(buf, n, inst, "stbu");
	case STH:
		return GetMnemonicDFormLoadStore(buf, n, inst, "sth");
	case STHU:
		return GetMnemonicDFormLoadStore(buf, n, inst, "sthu");
	case STW:
		return GetMnemonicDFormLoadStore(buf, n, inst, "stw");
	case STWU:
		return GetMnemonicDFormLoadStore(buf, n, inst, "stwu");
	case B:
	{
		const char *suf1 = inst.iform.lk ? "l" : "";
		const char *suf2 = inst.iform.aa ? "a" : "";
		unsigned long long target_address = inst.iform.li << 2;
		return snprintf(buf, n, "b%s%s\t0x%llx", suf1, suf2, target_address);
	}
	case 31:
		switch(inst.xoform.xo){
		case OPCD31_XO_ADD:
			return GetMnemonicXOFormRTRARBOERc(buf, n, inst, "add");
		case OPCD31_XO_SUBF:
			return GetMnemonicXOFormRTRARBOERc(buf, n, inst, "subf");
		case OPCD31_XO_ADDC:
			return GetMnemonicXOFormRTRARBOERc(buf, n, inst, "addc");
		case OPCD31_XO_SUBFC:
			return GetMnemonicXOFormRTRARBOERc(buf, n, inst, "subfc");
		case OPCD31_XO_ADDE:
			return GetMnemonicXOFormRTRARBOERc(buf, n, inst, "adde");
		case OPCD31_XO_SUBFE:
			return GetMnemonicXOFormRTRARBOERc(buf, n, inst, "subfe");
		case OPCD31_XO_ADDME:
			return GetMnemonicXOFormRTRAOERc(buf, n, inst, "addme");
		case OPCD31_XO_SUBFME:
			return GetMnemonicXOFormRTRAOERc(buf, n, inst, "subfme");
		case OPCD31_XO_ADDZE:
			return GetMnemonicXOFormRTRAOERc(buf, n, inst, "addze");
		case OPCD31_XO_SUBFZE:
			return GetMnemonicXOFormRTRAOERc(buf, n, inst, "subfze");
		case OPCD31_XO_NEG:
			return GetMnemonicXOFormRTRAOERc(buf, n, inst, "neg");
		default:
			break;
		}
		switch(inst.xform.xo){
		case OPCD31_X_AND:
			return GetMnemonicXFormRARSRBRc(buf, n, inst, "and");
		case OPCD31_X_OR:
			if(inst.xform.rt == inst.xform.rb){
				return snprintf(buf, n, "mr\t%u, %u", inst.xform.ra, inst.xform.rb);
			}
			return GetMnemonicXFormRARSRBRc(buf, n, inst, "or");
		case OPCD31_X_XOR:
			return GetMnemonicXFormRARSRBRc(buf, n, inst, "xor");
		case OPCD31_X_NAND:
			return GetMnemonicXFormRARSRBRc(buf, n, inst, "nand");
		case OPCD31_X_NOR:
			if(inst.xform.rt == inst.xform.rb){
				return snprintf(buf, n, "not\t%u, %u", inst.xform.ra, inst.xform.rt);
			}
			return GetMnemonicXFormRARSRBRc(buf, n, inst, "nor");
		case OPCD31_X_EQV:
			return GetMnemonicXFormRARSRBRc(buf, n, inst, "eqv");
		case OPCD31_X_ANDC:
			return GetMnemonicXFormRARSRBRc(buf, n, inst, "andc");
		case OPCD31_X_ORC:
			return GetMnemonicXFormRARSRBRc(buf, n, inst, "orc");
		case OPCD31_X_EXTSB:
			return GetMnemonicXFormRARSRc(buf, n, inst, "extsb");
		case OPCD31_X_EXTSH:
			return GetMnemonicXFormRARSRc(buf, n, inst, "extsh");
		case OPCD31_X_CNTLZW:
			return GetMnemonicXFormRARSRc(buf, n, inst, "cntlzw");
		case OPCD31_X_CMPB:
			return snprintf(buf, n, "cmpb\t%u, %u, %u", inst.xform.ra, inst.xform.rt, inst.xform.rb);
		case OPCD31_X_POPCNTB:
			return GetMnemonicXFormRARS(buf, n, inst, "popcntb");
		case OPCD31_X_POPCNTW:
			return GetMnemonicXFormRARS(buf, n, inst, "popcntw");
		case OPCD31_X_SLW:
		case OPCD31_X_SRW:
		case OPCD31_X_SRAW:
		case OPCD31_X_SRAWI:
		default:
			break;
		}
		return -1;
	case 19:
		switch(inst.xlform.xo){
		case OPCD19_BCLR:
		{
			unsigned int bo = inst.xlform.bt;
			unsigned int bi = inst.xlform.ba;
			unsigned int bh = inst.xlform.bb & 3;
			if((bo & 0x14) == 0x14){
				return snprintf(buf, n, "blr");
			}
			if(inst.xlform.lk){
				return snprintf(buf, n, "bclr\t%u, %u, %u", bo, bi, bh);
			}
			else{
				return snprintf(buf, n, "bclrl\t%u, %u, %u", bo, bi, bh);
			}
		}
		case OPCD19_BCCTR:
			return -1;
		case OPCD19_CRAND:
			return GetMnemonicXLFormBTBABB(buf, n, inst, "crand");
		case OPCD19_CRNAND:
			return GetMnemonicXLFormBTBABB(buf, n, inst, "crnand");
		case OPCD19_CROR:
			return GetMnemonicXLFormBTBABB(buf, n, inst, "cror");
		case OPCD19_CRXOR:
			return GetMnemonicXLFormBTBABB(buf, n, inst, "crxor");
		case OPCD19_CRNOR:
			return GetMnemonicXLFormBTBABB(buf, n, inst, "crnor");
		case OPCD19_CREQV:
			return GetMnemonicXLFormBTBABB(buf, n, inst, "creqv");
		case OPCD19_CRANDC:
			return GetMnemonicXLFormBTBABB(buf, n, inst, "crandc");
		case OPCD19_CRORC:
			return GetMnemonicXLFormBTBABB(buf, n, inst, "crorc");
		default:
			return -1;
		}
	case SC:
		return snprintf(buf, n, "sc\t0x%x", inst.scform.lev);
	case LMW:
	case STMW:
	case 30:
	case CMPI:
	case CMPLI:
	case RLWINM:
	case RLWNM:
	case RLWIMI:
	default:
		return -1;
	}
}
