/*!
@file dkcSemiRealRandom.h
@brief		ほぼ真性乱数
@author		d金魚
@since		2005/02/20
@note
dkcMath.hに書いていたのを別ファイルにした。

参考文献：
http://www.interq.or.jp/snake/totugeki/hdrand.htm 2005/02/20　Accessed

上記URLより引用	

このプログラムを利用して発生したいかなる損害にも、作者とつげき東北は責任を負わない。各自の責任の下に使用すること。
改良、自作ソフトへの組み込み等は全て自由。このページの内容を著作権だけを変更して、そのまま自分のHPに書くようなことはしないように（著作権法に違反する）。
このページは2002/04/21に公開された。

hdrand.c Copyright(C) とつげき東北

*/
#ifndef DKUTIL_C_SEMI_REAL_RANDOM_H
#define DKUTIL_C_SEMI_REAL_RANDOM_H


#define dkcdSEMI_REAL_RANDOM_TRIAL_AND_ERROR_LIMIT 64

#define dkcdSEMI_REAL_RANDOM_BITS 64


#include "dkcGenericFileSystem.h"

///テンポラリファイルの名前
#define dkcdSRR_TEMP_FILE_NAME "secure_rand_temp.tmp\0"
#define dkcdSRR_TEMP_FILE_NAME_W L"secure_rand_temp.tmp\0"

typedef struct dkc_SemiRealRandom{
	DKC_GENERIC_FILESYSTEM *mpObj;
#ifdef WIN32
	BOOL mIsNT;
	void *pPath;
	DKC_DLL *pDLL;
	void *fpGetTempPath;
	void *fpGetTempFileName;
	void *fpQueryPerformanceCounter;
	uint32 mLeftShift;
	uint32 mRightShift;
#else

#endif

}DKC_SEMI_REAL_RANDOM;


/**
@note

*/
DKC_EXTERN DKC_SEMI_REAL_RANDOM *WINAPI dkcAllocSemiRealRandom();


DKC_EXTERN DKC_SEMI_REAL_RANDOM *WINAPI dkcAllocSemiRealRandomFromGFS(DKC_GENERIC_FILESYSTEM *);


DKC_EXTERN int WINAPI dkcFreeSemiRealRandom(DKC_SEMI_REAL_RANDOM **);

DKC_EXTERN int WINAPI dkcSemiRealRandomInit(DKC_SEMI_REAL_RANDOM *);

DKC_EXTERN BOOL WINAPI dkcSemiRealRandomGet32(DKC_SEMI_REAL_RANDOM *,uint32 *);

DKC_EXTERN uint8 WINAPI dkcSemiRealRandomGet1(DKC_SEMI_REAL_RANDOM *);









#endif //end of include once