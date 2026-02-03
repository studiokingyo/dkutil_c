/**
@author d金魚
@file x86.h
@brief x86系？のCPUの判別や専用処理　等
@since 2005/01/23
*/


#ifndef DKUTIL_CSTD_X86_H
#define DKUTIL_CSTD_X86_H

#include "os.h"
#include "define.h"
#include "stdbool.h"

enum edkX86_CPU_type{
	///過去の遺産
	edkcCPU_Legacy=0,
	//intel系
	edkcCPU_MMX=1,
	edkcCPU_MMX2 = 2,
	edkcCPU_SSE=4,
	edkcCPU_SSE2=8,
	//AMD系
	edkcCPU_3DNOW=32,
	edkcCPU_E3DNOW=64,
	edkcCPU_3DNOW_PRO=128,
};

typedef struct dkcstd_CPU_INFO{
	char CPUName[256];
	char CPUType[128];
	unsigned char bFPU;
	unsigned char bTSC;
	unsigned char bCMOV;
	unsigned char bFCMOV;
	unsigned char bCPUID;
	unsigned char bMMX;
	unsigned char bMMX2;
	unsigned char bSSE;
	unsigned char bSSE2;
	unsigned char b3DNOW;
	unsigned char bE3DNOW;
	long TypeID;
	long FamilyID;
	long ModelID;
	long SteppingID;
	long CPUClock;
}DKCSTD_CPU_INFO;

#if 1==DKUTIL_CSTD_MSVC_IASM







///CPUIDの存在を確認 @return あるのであれば TRUE
DKC_EXTERN int WINAPI dkcstd_IsCPUID();
DKC_EXTERN int WINAPI dkcstd_getCPUID();

///@return edkX86_CPU_typeで定義されているもの
DKC_EXTERN uint32 WINAPI dkcstd_getX86CPUType();

/**
@param dest[out] DKCSTD_CPU_INFO構造体にCPUの情報を格納する
@param CPUClockCalcTime[in] CPUのクロックを測定する時間 1000(1秒)くらいが丁度よい。0だと測定しない。
@return TRUEで成功
*/
///CPUの状態を取得する。
DKC_EXTERN BOOL WINAPI dkcstd_getX86CPUInfo(DKCSTD_CPU_INFO *dest,int CPUClockCalcTime);

///@note FPUが使える事
DKC_EXTERN uint16 WINAPI dkcstd_getFPUMode();

///@note FPUが使える事
DKC_EXTERN int WINAPI dkcstd_setFPUMode(uint16);

DKC_EXTERN void WINAPI dkcstd_getRDTSC(DKC_UINT64_STRUCT *p);


DKC_INLINE DKC_UINT64_STRUCT dkcstd_getRDTSC_Fast()
{
	DKC_UINT64_STRUCT cy;	
	__asm {
    cpuid
    rdtsc
    mov cy.LowPart,  eax
    mov cy.HighPart, edx
	}
	return cy;
}

DKC_INLINE void dkcstd_emms(){
	_asm{
		emms
	}
}

#endif

#endif //end of include once

