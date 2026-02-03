
/*!
@file dkcVernam.c
@brief  íPèÉÇ» Varnam à√çÜ
@author dã‡ãõ
*/
#define DKUTIL_C_VERNAM_C
#include "dkcVernam.h"
#include "dkcMath.h"
#include "dkcStdio.h"




static WINAPIV dkcRandomV(ULONG *seed,ULONG Max_){
	return dkcRandom(seed,Max_);
}

void WINAPI dkcVernamEncrypt(BYTE *buffer,size_t size,const BYTE *key,size_t keysize,
														 ULONG seed,DKC_VERNAM_F_TYPE ff)
{
	size_t i;
	int pos;
	if(NULL==ff){
		ff = dkcRandomV;
	}
	for(i=0;i<size;i++){
		pos = ff(&seed,keysize);
		buffer[i] ^= key[pos];
	}
}

/*
static int WINAPI dkcVernamEncrypt2_Base(BYTE *buffer,size_t size,const BYTE *key,size_t keysize,
															const char *op)
{
	size_t i,j,k;// i=buffer j = key k = op 
	size_t opl = strlen(op);

	for(i=0,j=0,k=0;
		i<size;
		i++,j++,k++)
	{
		if(k<opl){
			k = 0;
		}
		if(j<keysize){
			j = 0;
		}
		switch(op[k]){
		case '+':
			buffer[i] += key[j];
			break;
		case '-':
			buffer[i] -= key[j];
			break;
		case '^':
			buffer[i] ^= key[j];
			break;
		default:
			return edk_FAILED;
		}
	}
	return edk_SUCCEEDED;
}
*/



void WINAPI dkcEasyVernamEncrypt(BYTE *buffer,size_t size,ULONG seed,DKC_VERNAM_F_TYPE ff)
{
	size_t i;
	BYTE num;
	if(NULL==ff){
		ff = dkcRandomV;
	}
	for(i=0;i<size;i++){
		num = (BYTE)(ff(&seed,UCHAR_MAX));
		buffer[i] ^= num;
	}
}








