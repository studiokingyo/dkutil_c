/*!
@file dkcBlowfish.h
@brief Blowfish encryptio algorithm
@author converted C by d金魚
@since 2004/07/07
@note
Bruce Schneier氏、Jim Conger氏に感謝します。
@todo
インターフェイスの変更の可能性あり。しかし、よほど気が向かない限り行わないだろう。
*/



#ifndef DKUTIL_C_BLOWFISH_H
#define DKUTIL_C_BLOWFISH_H

#include "dkcOSIndependent.h"

///@todo このdefineは変える可能性ありdkcd_BLOWFISH_MAX_KEY_SIZEを使用すべし
#define MAXKEYBYTES 	56		// 448 bits max
///@todo このdefineは変える可能性あり
#define NPASS           16		// SBox passes

///キーの最大値
#define dkcd_BLOWFISH_MAX_KEY_SIZE MAXKEYBYTES




typedef DWORD		(*DKC_BLOWFISH_SBOX_TYPE)[256];

typedef struct dkc_Blowfish{
	DWORD 		* PArray ;
	//DWORD		(* SBoxes)[256];
	DKC_BLOWFISH_SBOX_TYPE SBoxes;
}DKC_BLOWFISH;

/*!
@param key[in] キーへのポインタ
@param keysize[in] keyのサイズ
@note
dkcd_BLOWFISH_MAX_KEY_SIZEよりキーが大きい場合は失敗します。
内部でdkcBlowfishInit()を呼び出しています。
*/
DKC_EXTERN DKC_BLOWFISH *WINAPI dkcAllocBlowfish(BYTE *key,int keysize);

///dkcAllocBlowfish()で確保したメモリ領域を開放
DKC_EXTERN int WINAPI dkcFreeBlowfish(DKC_BLOWFISH **);
/*!
@note
- dkcd_BLOWFISH_MAX_KEY_SIZEよりキーが大きい場合は失敗します。
- この関数の使用は奨励されていません。
- この関数の名前は変更する場合があります。
@return edk_SUCCEEDEDなら成功
*/
///dkcAllocBlowfish()で初期化するけど、明示的に初期化したい場合これを呼び出す。
DKC_EXTERN int WINAPI dkcBlowfishInit(DKC_BLOWFISH *p,BYTE *key, int keybytes);


DKC_EXTERN DWORD WINAPI dkcBlowfishGetOutputLength (DWORD lInputLong);
/*!
Encode pIntput into pOutput.  Input length in lSize.  Returned value
is length of output which will be even MOD 8 bytes.  Inputbuffer and
output buffer can be the same, but be sure buffer length is even MOD8.
@return 0 == ERROR
*/
DKC_EXTERN DWORD WINAPI dkcBlowfishEncrypt(DKC_BLOWFISH *p,BYTE * pInput, BYTE * pOutput, DWORD lSize);
/*!
 Decode pIntput into pOutput.  Input length in lSize.  Inputbuffer and
 output buffer can be the same, but be sure buffer length is even MOD8.
*/
DKC_EXTERN void WINAPI dkcBlowfishDecrypt (DKC_BLOWFISH *p,BYTE * pInput, BYTE * pOutput, DWORD lSize);


/**
@note
dkcOSIndependent.hに移動
@code

/// choose a byte order for your hardware
#define ORDER_DCBA	// chosing Intel in this case

#ifdef ORDER_DCBA  	// DCBA - little endian - intel
	union aword {
	  DWORD dword;
	  BYTE byte [4];
	  struct {
	    unsigned int byte3:8;
	    unsigned int byte2:8;
	    unsigned int byte1:8;
	    unsigned int byte0:8;
	  } w;
	};
#endif

#ifdef ORDER_ABCD  	// ABCD - big endian - motorola
	union aword {
	  DWORD dword;
	  BYTE byte [4];
	  struct {
	    unsigned int byte0:8;
	    unsigned int byte1:8;
	    unsigned int byte2:8;
	    unsigned int byte3:8;
	  } w;
	};
#endif

#ifdef ORDER_BADC  	// BADC - vax
	union aword {
	  DWORD dword;
	  BYTE byte [4];
	  struct {
	    unsigned int byte1:8;
	    unsigned int byte0:8;
	    unsigned int byte3:8;
	    unsigned int byte2:8;
	  } w;
};
#endif
@endcode
*/

#if !defined(  DKUTIL_C_BLOWFISH_C ) &&  defined(USE_DKC_INDEPENDENT_INCLUDE)
#	include "dkcBlowfish.c"
#endif

#endif //end of include once