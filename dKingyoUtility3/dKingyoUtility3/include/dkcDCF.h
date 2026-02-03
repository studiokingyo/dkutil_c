/*!
@file dkcDCF.h
@brief dKingyo Cryption File
@note
dKingyo Cryption File形式の実装
@since 2005/11/22
*/
#ifndef DKUTIL_C_DCF_H
#define DKUTIL_C_DCF_H

#include "dkcDCF01.h"


DKC_EXTERN int WINAPI dkcFreeDCF(DKC_DCF **pp);

/**
内部メッセージダイジェスト関数を初期化する
*/
DKC_INLINE void WINAPI dkcDCFInit(DKC_DCF *p)
{
	p->fpInit(p);
}


DKC_INLINE int WINAPI dkcDCFWrite(DKC_DCF *p,const void *data,size_t size,size_t *write_size)
{
	return p->fpWrite(p,data,size,write_size);
}

DKC_INLINE int WINAPI dkcDCFWriteFinal(DKC_DCF *p)
{
	return p->fpWriteFinal(p);
}

DKC_INLINE int WINAPI dkcDCFRead(DKC_DCF *p,void *buff,size_t size,size_t *readsize)
{
	return p->fpRead(p,buff,size,readsize);
}


DKC_INLINE int WINAPI dkcDCFReadFinal(DKC_DCF *p)
{
	return p->fpReadFinal(p);
}


DKC_EXTERN BOOL WINAPI dkcDCFPasswordCheck(DKC_DCF *p,const char *filename,const void *password,size_t size);

DKC_EXTERN int WINAPI dkcFileToDCF(uint32 flags,const char *save_filename,const char *target_filename,
					 const void *key,size_t keysize);

DKC_EXTERN int WINAPI dkcDCFToFile(uint32 flags,const char *dest_filename,const char *target_filename,
					 const void *key,size_t keysize);

/**
すべて読み込みます。
内部でdkcDCFInit()されます。
*/
DKC_EXTERN int WINAPI dkcDCFAllRead(DKC_DCF *p,void *buff,size_t size);
/**
@return edk_SUCCEEDEDで成功。それ以外はエラー。DCF形式じゃない場合が濃厚な場合はedk_LogicExceptionを返します。
*/
///元のファイルサイズを予測します。
DKC_EXTERN BOOL WINAPI dkcDCFFileSizeExpect(const char *filename,uint64 *pv);

#endif //end of include once