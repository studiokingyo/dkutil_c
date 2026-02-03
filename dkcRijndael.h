/*!
@file dkcRijndael.h
@brief Rijndael Algorithm Wrapper
@author wrapped by d金魚
@note
http://fp.gladman.plus.com/cryptography_technology/index.htm
からDLできるRijndaelライブラリを使う場合（dkcRijndael側では未対応）
	dkcdBRIAN_GLADMAN_RIJNDAEL

デフォルトでrijndael-api-fstおよびrijndael-alg-fstを使用する。

*/



#ifndef dkutil_c_Rijndael
#define dkutil_c_Rijndael

#include "dkcOSIndependent.h"
#include "dkcMemoryStream.h"


#ifdef dkcdBRIAN_GLADMAN_RIJNDAEL


#include "rijndael/aes.h"



#define RIJNDAEL_BLOCK_SIZE BLOCK_SIZE

#endif

typedef struct dkc_RijnDael{
	void *mKey;
	void *mRijndael;
}DKC_RIJNDAEL;

enum edk_Rijndael{
	///  Key direction is invalid, e.g., unknown value 
	edkcBAD_KEY_DIR    =      -1 ,
	///  Key material not of correct length 
	edkcBAD_KEY_MAT     =     -2 ,
	///  Key passed is not valid 
	edkcBAD_KEY_INSTANCE =    -3 ,
	///  Params struct passed to cipherInit invalid 
	edkcBAD_CIPHER_MODE =     -4 ,
	///  Cipher in wrong state (e.g., not initialized) 
	edkcBAD_CIPHER_STATE =    -5 ,
	edkcBAD_BLOCK_LENGTH  =   -6,
	edkcBAD_CIPHER_INSTANCE = -7,
	///  Data contents are invalid, e.g., invalid padding 
	edkcBAD_DATA        =     -8 ,
	///  Unknown error 
	edkcBAD_OTHER       =     -9,
	///暗号化するとき～～
	edkcRijndaelEncrypt = 0,
	///復号化するとき～～
	edkcRijndaelDecrypt = 1,
	///  Are we ciphering in ECB mode?   
	edkcRijndael_ECB = 1,
	///  Are we ciphering in CBC mode?   
	edkcRijndael_CBC = 2,
	///  Are we ciphering in 1-bit CFB mode?
	edkcRijndael_CFB1 = 3,
};
//enum{


//};


DKC_EXTERN DKC_RIJNDAEL *WINAPI dkcAllocRijndael();

DKC_EXTERN DKC_RIJNDAEL *WINAPI dkcAllocRijndaelEncrypt(const BYTE *key,size_t keysize);



DKC_EXTERN DKC_RIJNDAEL *WINAPI dkcAllocRijndaelDecrypt(const BYTE *key,size_t keysize);


//int WINAPI dkcRijnDaelCrypt(

DKC_EXTERN int WINAPI dkcFreeRijndael(DKC_RIJNDAEL **);

DKC_EXTERN int WINAPI dkcRijndaelBlockEncrypt(DKC_RIJNDAEL *p,BYTE *dest,int dsize,const BYTE *src,int ssize);


DKC_EXTERN int WINAPI dkcRijndaelBlockDecrypt(DKC_RIJNDAEL *p,BYTE *dest,int dsize,const BYTE *src,int ssize);

DKC_EXTERN int WINAPI dkcRijndaelPadEncrypt(DKC_RIJNDAEL *p,BYTE *dest,int dsize,const BYTE *src,int ssize);


DKC_EXTERN int WINAPI dkcRijndaelPadDecrypt(DKC_RIJNDAEL *p,BYTE *dest,int dsize,const BYTE *src,int ssize);

DKC_EXTERN int WINAPI dkcRijndaelStringKey(DKC_RIJNDAEL *p,
																					 char *dest_and_material,int size,BYTE option);

#define dkcRijndaelEncodeStringKey(a,b,c) dkcRijndaelStringKey(a,b,c,edkcRijndaelEncrypt)

#define dkcRijndaelEDecodeStringKey(a,b,c) dkcRijndaelStringKey(a,b,c,edkcRijndaelDecrypt)

///@todo まだ未実装な為、実装する事
DKC_EXTERN BOOL WINAPI dkcRijndaelErrorMessage(int result,char *buff,size_t size);

//DKC_EXTERN int WINAPI dkcRijndaelDecodeStringKey(DKC_RIJNDAEL *p,char *dest_and_material,size_t size);







#endif //end of include once