#ifndef _INSTRUCTIONFORMAT_H_
#define _INSTRUCTIONFORMAT_H_

/*
 * Power Instruction Set Architecture Version 2.06
 * 1.6 Instruction formats
*/


typedef union{
	struct{
		unsigned int other : 26;
		unsigned int opcd : 6;
	} common;
	struct{
		unsigned int rc : 1;
		unsigned int xo : 10;
		unsigned int rb : 5;
		unsigned int ra : 5;
		unsigned int rt : 5;
		unsigned int opcd : 6;
	} xform;
	struct{
		unsigned int imm : 16;
		unsigned int ra : 5;
		unsigned int rt : 5;
		unsigned int opcd : 6;
	} dform;
	struct{
		unsigned int rc : 1;
		unsigned int xo : 9;
		unsigned int oe : 1;
		unsigned int rb : 5;
		unsigned int ra : 5;
		unsigned int rt : 5;
		unsigned int opcd : 6;
	} xoform;
	struct{
		unsigned int b: 1;
		unsigned int xo : 10;
		unsigned int r: 10;
		unsigned int rt : 5;
		unsigned int opcd : 6;
	} xfxform;
	struct {
		unsigned int lk : 1;
		unsigned int aa : 1;
		unsigned int li : 24;
		unsigned int opcd : 6;
	} iform;
	struct {
		unsigned int lk : 1;
		unsigned int xo : 10;
		unsigned int bb : 5;
		unsigned int ba : 5;
		unsigned int bt : 5;
		unsigned int opcd : 6;
	} xlform;
	struct {
		unsigned int lk : 1;
		unsigned int aa : 1;
		unsigned int bd : 14;
		unsigned int bi : 5;
		unsigned int bo : 5;
		unsigned int opcd : 6;
	} bform;
	struct {
		unsigned int rc : 1;
		unsigned int me : 5;
		unsigned int mb : 5;
		unsigned int rb : 5;
		unsigned int ra : 5;
		unsigned int rs : 5;
		unsigned int opcd : 6;
	} mform;
	struct {
		unsigned int rc : 1;
		unsigned int sh2 : 1;
		unsigned int xo : 3;
		unsigned int mb : 6;
		unsigned int sh1 : 5;
		unsigned int ra : 5;
		unsigned int rs : 5;
		unsigned int opcd : 6;
	} mdform;
	struct {
		unsigned int rc : 1;
		unsigned int xo : 4;
		unsigned int mb : 6;
		unsigned int rb : 5;
		unsigned int ra : 5;
		unsigned int rs : 5;
		unsigned int opcd : 6;
	} mdsform;
	struct {
		unsigned int xo : 2;
		unsigned int ds : 14;
		unsigned int ra : 5;
		unsigned int rt : 5;
		unsigned int opcd : 6;
	} dsform;
	struct {
		unsigned int field6 : 1;
		unsigned int field5 : 1;
		unsigned int field4 : 3;
		unsigned int lev : 7;
		unsigned int field3 : 4;
		unsigned int field2 : 5;
		unsigned int field1 : 5;
		unsigned int opcd : 6;
	} scform;
	unsigned int raw;
} inst_t;

#endif
