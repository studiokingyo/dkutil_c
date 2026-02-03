/**
@auther dã‡ãõ
@brief Visual C++ inline assembler misc

*/
#ifndef DKUTIL_C_VC_ASM_MISC_H
#define DKUTIL_C_VC_ASM_MISC_H



/**
@param m[out] MMX register
@param a[in] double word 
*/
#define MMX_REGISTER_PUSH32(m,a)\
	_asm	movd m,a\
	_asm	punpckldq m,m

/**
@param m[in][out] MMX register
@param a[out] â∫à  double word m
@parma b[out] è„à  double word m
*/
#define MMX_REGISTER_GET32(m,a,b)\
	_asm movd a,m\
	_asm PUNPCKHDQ m,m\
	_asm movd b,m

#define SWAP_ASM32(a,b)\
	_asm mov eax,a\
	_asm mov a,b\
	_asm mov b,eax

#endif 
