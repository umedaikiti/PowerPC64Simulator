#ifndef _INSTRUCTIONS_H_
#define _INSTRUCTIONS_H_

#include "PowerPC64.h"

#define RET_NOTIMPLEMENTED (-1)
#define RET_SUCCESS 0
#define RET_INVALIDINSTRUCTION (-2)
#define RET_MEMORYERROR (-3)

#define SUBFIC (8)
#define ADDIC (12)
#define ADDIC_cr0 (13)
#define ADDI (14)
#define ADDIS (15)
#define ANDI (28)
#define ORI (24)
#define ANDIS (29)
#define ORIS (25)
#define XORI (26)
#define XORIS (27)
#define CMPI (11)
#define CMPLI (10)
#define RLWINM (21)
#define RLWNM (23)
#define RLWIMI (20)
#define LBZ (34)
#define LBZU (35)
#define LHZ (40)
#define LHZU (41)
#define LHA (42)
#define LHAU (43)
#define LWZ (32)
#define LWZU (33)
#define STB (38)
#define STBU (39)
#define STH (44)
#define STHU (45)
#define STW (36)
#define STWU (37)
#define LMW (46)
#define STMW (47)
#define BC (16)
#define B (18)
#define RLWINM (21)
#define RLWNM (23)
#define RLWIMI (20)
#define SC (17)

/* arith_xo (OPCD = 34) */
#define OPCD31_XO_ADD (266)
#define OPCD31_XO_SUBF (40)
#define OPCD31_XO_ADDC (10)
#define OPCD31_XO_SUBFC (8)
#define OPCD31_XO_ADDE (138)
#define OPCD31_XO_SUBFE (136)
#define OPCD31_XO_ADDME (234)
#define OPCD31_XO_SUBFME (232)
#define OPCD31_XO_ADDZE (202)
#define OPCD31_XO_SUBFZE (200)
#define OPCD31_XO_NEG (104)

/* logical_x (OPCD = 34) */
#define OPCD31_X_AND (28)
#define OPCD31_X_OR (444)
#define OPCD31_X_XOR (316)
#define OPCD31_X_NAND (476)
#define OPCD31_X_NOR (124)
#define OPCD31_X_EQV (284)
#define OPCD31_X_ANDC (60)
#define OPCD31_X_ORC (412)
#define OPCD31_X_EXTSB (954)
#define OPCD31_X_EXTSH (922)
#define OPCD31_X_CNTLZW (26)
#define OPCD31_X_CMPB (508)
#define OPCD31_X_POPCNTB (122)
#define OPCD31_X_POPCNTW (378)
#define OPCD31_X_PRTYD (186)

#define OPCD31_X_SLW (24)
#define OPCD31_X_SRW (536)
#define OPCD31_X_SRAWI (824)
#define OPCD31_X_SRAW (792)

#define OPCD31_X_STBX (215)
#define OPCD31_X_STBUX (247)
#define OPCD31_X_STHX (407)
#define OPCD31_X_STHUX (439)
#define OPCD31_X_STWX (151)
#define OPCD31_X_STWUX (183)
#define OPCD31_X_STDX (149)
#define OPCD31_X_STDUX (181)


//#define OPCD31_X_ ()
//#define OPCD31_XO_ ()

#define OPCD19_BCLR (16)
#define OPCD19_BCCTR (528)
#define OPCD19_CRAND (257)
#define OPCD19_CRNAND (255)
#define OPCD19_CROR (449)
#define OPCD19_CRXOR (193)
#define OPCD19_CRNOR (33)
#define OPCD19_CREQV (289)
#define OPCD19_CRANDC (129)
#define OPCD19_CRORC (417)

#define OPCD30_MD_RLDICL (0)
#define OPCD30_MD_RLDICR (1)
#define OPCD30_MD_RLDIC (2)
#define OPCD30_MD_RLDIMI (3)
#define OPCD30_MDS_RLDCL (8)
#define OPCD30_MDS_RLDCR (9)


int addic(ppc64_t *ppc, inst_t inst);
int addi(ppc64_t *ppc, inst_t inst);
int addis(ppc64_t *ppc, inst_t inst);
int andi(ppc64_t *ppc, inst_t inst);
int andis(ppc64_t *ppc, inst_t inst);
int ori(ppc64_t *ppc, inst_t inst);
int oris(ppc64_t *ppc, inst_t inst);
int xori(ppc64_t *ppc, inst_t inst);
int xoris(ppc64_t *ppc, inst_t inst);

int and(ppc64_t *ppc, inst_t inst);
int or(ppc64_t *ppc, inst_t inst);
int xor(ppc64_t *ppc, inst_t inst);
int nand(ppc64_t *ppc, inst_t inst);
int nor(ppc64_t *ppc, inst_t inst);
int eqv(ppc64_t *ppc, inst_t inst);
int andc(ppc64_t *ppc, inst_t inst);
int orc(ppc64_t *ppc, inst_t inst);
int extsb(ppc64_t *ppc, inst_t inst);

int cmpb(ppc64_t *ppc, inst_t inst);
int cmpi(ppc64_t *ppc, inst_t inst);
int cmpli(ppc64_t *ppc, inst_t inst);

int popcntb(ppc64_t *ppc, inst_t inst);
int popcntw(ppc64_t *ppc, inst_t inst);

int cntlzw(ppc64_t *ppc, inst_t inst);
int extsh(ppc64_t *ppc, inst_t inst);

int adde(ppc64_t *ppc, inst_t inst);
int addc(ppc64_t *ppc, inst_t inst);
int subfc(ppc64_t *ppc, inst_t inst);
int subfe(ppc64_t *ppc, inst_t inst);
int addme(ppc64_t *ppc, inst_t inst);
int subfme(ppc64_t *ppc, inst_t inst);
int addze(ppc64_t *ppc, inst_t inst);
int subfze(ppc64_t *ppc, inst_t inst);
int neg(ppc64_t *ppc, inst_t inst);
int b(ppc64_t *ppc, inst_t inst);
int bc(ppc64_t *ppc, inst_t inst);

int rlwinm(ppc64_t *ppc, inst_t inst);
int rlwnm(ppc64_t *ppc, inst_t inst);
int rlwimi(ppc64_t *ppc, inst_t inst);
int rldicl(ppc64_t *ppc, inst_t inst);
int rldicr(ppc64_t *ppc, inst_t inst);
int rldic(ppc64_t *ppc, inst_t inst);
int rldcl(ppc64_t *ppc, inst_t inst);
int rldcr(ppc64_t *ppc, inst_t inst);
int rldimi(ppc64_t *ppc, inst_t inst);

int stb(ppc64_t *ppc, inst_t inst);
int stbx(ppc64_t *ppc, inst_t inst);
int stbu(ppc64_t *ppc, inst_t inst);
int stbux(ppc64_t *ppc, inst_t inst);
int sth(ppc64_t *ppc, inst_t inst);
int sthx(ppc64_t *ppc, inst_t inst);
int sthu(ppc64_t *ppc, inst_t inst);
int sthux(ppc64_t *ppc, inst_t inst);
int stw(ppc64_t *ppc, inst_t inst);
int stwx(ppc64_t *ppc, inst_t inst);
int stwu(ppc64_t *ppc, inst_t inst);
int stwux(ppc64_t *ppc, inst_t inst);
int std(ppc64_t *ppc, inst_t inst);
int stdx(ppc64_t *ppc, inst_t inst);
int stdu(ppc64_t *ppc, inst_t inst);
int stdux(ppc64_t *ppc, inst_t inst);

int lbz(ppc64_t *ppc, inst_t inst);
int lbzx(ppc64_t *ppc, inst_t inst);
int lbzu(ppc64_t *ppc, inst_t inst);
int lbzux(ppc64_t *ppc, inst_t inst);
int lhz(ppc64_t *ppc, inst_t inst);
int lhzx(ppc64_t *ppc, inst_t inst);
int lhzu(ppc64_t *ppc, inst_t inst);
int lhzux(ppc64_t *ppc, inst_t inst);
int lha(ppc64_t *ppc, inst_t inst);
int lhax(ppc64_t *ppc, inst_t inst);
int lhau(ppc64_t *ppc, inst_t inst);
int lhaux(ppc64_t *ppc, inst_t inst);
int lwz(ppc64_t *ppc, inst_t inst);
int lwzx(ppc64_t *ppc, inst_t inst);
int lwzu(ppc64_t *ppc, inst_t inst);
int lwzux(ppc64_t *ppc, inst_t inst);
int lwa(ppc64_t *ppc, inst_t inst);
int lwax(ppc64_t *ppc, inst_t inst);
int lwaux(ppc64_t *ppc, inst_t inst);
int ld(ppc64_t *ppc, inst_t inst);
int ldx(ppc64_t *ppc, inst_t inst);
int ldu(ppc64_t *ppc, inst_t inst);
int ldux(ppc64_t *ppc, inst_t inst);
int sc(ppc64_t *ppc, inst_t inst);

int opcd31(ppc64_t *ppc, inst_t inst);
int opcd19(ppc64_t *ppc, inst_t inst);
int opcd30(ppc64_t *ppc, inst_t inst);

int slw(ppc64_t *ppc, inst_t inst);
int srw(ppc64_t *ppc, inst_t inst);
int srawi(ppc64_t *ppc, inst_t inst);
int sraw(ppc64_t *ppc, inst_t inst);
int mtspr(ppc64_t *ppc, inst_t inst);
int mtcrf(ppc64_t *ppc, inst_t inst);
int mfcr(ppc64_t *ppc, inst_t inst);
int mtocrf(ppc64_t *ppc, inst_t ins);
int mfocrf(ppc64_t *ppc, inst_t inst);
int mcrxr(ppc64_t *ppc, inst_t inst);
int mtdcrux(ppc64_t *ppc, inst_t inst);
int mfdcrux(ppc64_t *ppc, inst_t inst);

int mulli(ppc64_t *ppc, inst_t inst);
int mulhw(ppc64_t *ppc, inst_t inst);
int mullw(ppc64_t *ppc, inst_t inst);
int mulhwu(ppc64_t *ppc, inst_t inst);
int divw(ppc64_t *ppc, inst_t inst);
int divwu(ppc64_t *ppc, inst_t inst);
int divwe(ppc64_t *ppc, inst_t inst);
int divweu(ppc64_t *ppc, inst_t inst);
int mulld(ppc64_t *ppc, inst_t inst);
int mulhd(ppc64_t *ppc, inst_t inst);
int mulhdu(ppc64_t *ppc, inst_t inst);
int divd(ppc64_t *ppc, inst_t inst);
int divdu(ppc64_t *ppc, inst_t inst);
int divde(ppc64_t *ppc, inst_t inst);
int divdeu(ppc64_t *ppc, inst_t inst);

int crand(ppc64_t *ppc, inst_t inst);
int crnand(ppc64_t *ppc, inst_t inst);
int cror(ppc64_t *ppc, inst_t inst);
int crxor(ppc64_t *ppc, inst_t inst);
int crnor(ppc64_t *ppc, inst_t inst);
int creqv(ppc64_t *ppc, inst_t inst);
int crandc(ppc64_t *ppc, inst_t inst);
int crorc(ppc64_t *ppc, inst_t inst);
int mcrf(ppc64_t *ppc, inst_t inst);

#endif
