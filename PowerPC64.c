
#include "InstructionFormat.h"
#include "Instructions.h"
#include "PowerPC64.h"
#include "Mnemonic.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

//#define TARGET_LITTLEENDIAN

//ppc64_tの初期化
//レジスタ類を0で埋める
//プログラムをメモリにロード
//CIA, NIAのセット
int InitPPC(ppc64_t *ppc, const char *filename)
{
	struct stat buf;
	int fd;
	unsigned char *p;
	if(stat(filename, &buf) == -1) return -1;
	fd = open(filename, O_RDONLY);
	if(fd < 0) return -1;
	p = malloc(buf.st_size);
	if(p == NULL) return -1;
	if(read(fd, p, buf.st_size) < 0) return -1;
	close(fd);

	memset(ppc, 0, sizeof(ppc64_t));
	WriteMemory(ppc, 0, p, buf.st_size);
//	WriteMemory(ppc, entrypoint, p, buf.st_size);
	free(p);
//	ppc->cia = entrypoint;
	ppc->nia = ppc->cia + 4;
	return 0;
}

//n >= 14
//CRレジスタの内容を文字列にして返す
static inline int ConditionRegister(char *buf, size_t n, unsigned int cr)
{
	const char *lt = (cr & 8) != 0 ? "LT" : "lt";
	const char *gt = (cr & 4) != 0 ? "GT" : "gt";
	const char *eq= (cr & 2) != 0 ? "EQ" : "eq";
	const char *so = (cr & 1) != 0 ? "SO" : "so";
	return snprintf(buf, n, "[%s|%s|%s|%s]", lt, gt, eq, so);
}

//全てのレジスタの内容を表示する
void PrintRegisters(ppc64_t *ppc)
{
	int i;
	unsigned int ca, ov, so;
	char buf[16];
	ca = ppc->regs.XER.field.ca;
	ov = ppc->regs.XER.field.ov;
	so = ppc->regs.XER.field.so;
	printf("Registers\n");
	for(i=0;i<32;i++){
		printf("r%d\t%016llx\n", i, ppc->regs.GPR[i]);
	}
	printf("lr\t%016llx\n", ppc->regs.LR);
	printf("ctr\t%016llx\n", ppc->regs.CTR);
	ConditionRegister(buf, sizeof(buf), ppc->regs.CR.field.cr0);
	printf("cr0\t%s\n", buf);
	ConditionRegister(buf, sizeof(buf), ppc->regs.CR.field.cr1);
	printf("cr1\t%s\n", buf);
	ConditionRegister(buf, sizeof(buf), ppc->regs.CR.field.cr2);
	printf("cr2\t%s\n", buf);
	ConditionRegister(buf, sizeof(buf), ppc->regs.CR.field.cr3);
	printf("cr3\t%s\n", buf);
	ConditionRegister(buf, sizeof(buf), ppc->regs.CR.field.cr4);
	printf("cr4\t%s\n", buf);
	ConditionRegister(buf, sizeof(buf), ppc->regs.CR.field.cr5);
	printf("cr5\t%s\n", buf);
	ConditionRegister(buf, sizeof(buf), ppc->regs.CR.field.cr6);
	printf("cr6\t%s\n", buf);
	ConditionRegister(buf, sizeof(buf), ppc->regs.CR.field.cr7);
	printf("cr7\t%s\n", buf);
	printf("XER\t%s %s %s\n",
			ca ? "CA" : "ca",
			ov ? "OV" : "ov",
			so ? "SO" : "so");
}

const char *help =
		"h: print this help\n"
		"r <num>: print GPR[<num>]\n"
		"n: execute next instruction\n"
		"d: dump all registers\n"
		"q: quit\n";

//ユーザーからの入力を受け取り、応答する
int Command(ppc64_t *ppc, const char* cmd)
{
	switch(cmd[0]){
	case 'h':
		printf("%s", help);
		return 0;
	case 'r':
	{
		int n, ret;
		if((ret = sscanf(cmd, "r %d", &n)) != 1){
			printf("sscanf error: %d\n", ret);
			return 0;
		}
		if(0 <= n && n < 32){
			printf("r%d : %016llx\n", n, ppc->regs.GPR[n]);
		}
		else{
			printf("Invalid command\n");
		}
	}
	return 0;
	case 'n':
		return 1;
	case 'd':
		PrintRegisters(ppc);
		return 0;
	case 'q':
		return -1;
	default:
		return 0;
	}
}

//CIAにある命令を返す
inst_t NextInstruction(ppc64_t *ppc)
{
	inst_t inst;
	unsigned int data;
	ReadMemory4(ppc, ppc->cia, &data);
	inst.raw = data;
	return inst;
}

//命令を実行する
//opcdに応じてInstructions.hの関数を呼び出す
int Execute(ppc64_t *ppc, inst_t instruction)
{
	switch(instruction.common.opcd){
	case ADDI:
		return addi(ppc, instruction);
	case ADDIS:
		return addis(ppc, instruction);
	case ADDIC:
	case ADDIC_cr0:
		return addic(ppc, instruction);
	case ANDI:
		return andi(ppc, instruction);
	case ANDIS:
		return andis(ppc, instruction);
	case ORI:
		return ori(ppc, instruction);
	case ORIS:
		return oris(ppc, instruction);
	case XORI:
		return xori(ppc, instruction);
	case XORIS:
		return xoris(ppc, instruction);
	case CMPI:
		return cmpi(ppc, instruction);
	case CMPLI:
		return cmpli(ppc, instruction);
	case B:
		return b(ppc, instruction);
	case BC:
		return bc(ppc, instruction);
	case LBZ:
		return lbz(ppc, instruction);
	case LBZU:
		return lbzu(ppc, instruction);
	case LHZ:
		return lhz(ppc, instruction);
	case LHZU:
		return lhzu(ppc, instruction);
	case LHA:
		return lha(ppc, instruction);
	case LHAU:
		return lhau(ppc, instruction);
	case LWZ:
		return lwz(ppc, instruction);
	case LWZU:
		return lwzu(ppc, instruction);
	case STB:
		return stb(ppc, instruction);
	case STBU:
		return stbu(ppc, instruction);
	case STH:
		return sth(ppc, instruction);
	case STHU:
		return sthu(ppc, instruction);
	case STW:
		return stw(ppc, instruction);
	case STWU:
		return stwu(ppc, instruction);
/*
	case LMW:
		return lmw(ppc, instruction);
	case STMW:
		return stmw(ppc, instruction);
*/
	case 19:
		return opcd19(ppc, instruction);
	case 30:
		return opcd30(ppc, instruction);
	case 31:
		return opcd31(ppc, instruction);
	case 58:
		switch(instruction.dsform.xo){
		case 0:
			return ld(ppc, instruction);
		case 1:
			return ldu(ppc, instruction);
		case 2:
			return lwa(ppc, instruction);
		default:
			return RET_NOTIMPLEMENTED;
		}
	case 62:
		switch(instruction.dsform.xo){
		case 0:
			return std(ppc, instruction);
		case 1:
			return stdu(ppc, instruction);
		default:
			return RET_NOTIMPLEMENTED;
		}
	case SC:
		return sc(ppc, instruction);
	default:
		return RET_NOTIMPLEMENTED;
	}
}

//シミュレーションを実行する関数
int PowerPC64MainLoop(ppc64_t *ppc)
{
	inst_t inst;
	int error = 0;
	while(1){
		char buf[16];
		char mnemonic[512];
		int ret = 0;
		while(ret == 0){
			printf("\ntype h for help\n> ");
			fgets(buf, sizeof(buf), stdin);
			ret = Command(ppc, buf);
		}
		if(ret == -1) break;
		//PrintRegisters(ppc);
		inst = NextInstruction(ppc);
		if(inst.raw == 0) break;

		if(GetMnemonic(mnemonic, sizeof(mnemonic), inst) < 0){
			strncpy(mnemonic, "mnemonic unknown", sizeof(mnemonic));
		}
		printf("0x%llx : %s\n", ppc->cia, mnemonic);

		error = Execute(ppc, inst);
		switch(error){
		case RET_SUCCESS:
			ppc->cia = ppc->nia;
			ppc->nia += 4;
			break;
		case RET_NOTIMPLEMENTED:
			printf("NotImplemented\n");
			return -1;
		case RET_INVALIDINSTRUCTION:
			printf("InvalidInstruction\n");
			return -2;
		}
	}
	return 0;
}

