/*! @file
	@brief scrypt パスワードハッシュ
	@author d.Kingyo
	@note
	scryptはColin Percival (2009)が設計したメモリハードな
	パスワードハッシュ関数です。

	特徴:
	- メモリハード（大量のメモリを要求）
	- CPUハード（計算コストが高い）
	- パラメータ: N（コスト）, r（ブロックサイズ）, p（並列度）

	参考: RFC 7914 - The scrypt Password-Based Key Derivation Function
*/

#ifndef DKC_SCRYPT_INC_
#define DKC_SCRYPT_INC_

#include "dkcOSIndependent.h"

#ifdef __cplusplus
extern "C" {
#endif

/* ====================================================================
 * 関数宣言
 * ==================================================================== */

/*!
	@brief scryptパスワードハッシュ
	@param pwd パスワード
	@param pwdlen パスワード長
	@param salt ソルト
	@param saltlen ソルト長
	@param N CPUコスト（2の累乗でなければならない）
	@param r ブロックサイズパラメータ
	@param p 並列度パラメータ
	@param output 出力バッファ
	@param outlen 出力長
	@return edk_SUCCEEDED:成功 edk_FAILED:失敗
	@note Nは2の累乗、r*p < 2^30 でなければならない
*/
DKC_EXTERN int WINAPI dkcScrypt(
	const unsigned char *pwd, size_t pwdlen,
	const unsigned char *salt, size_t saltlen,
	uint32 N, uint32 r, uint32 p,
	unsigned char *output, size_t outlen);

#ifdef __cplusplus
}
#endif

#endif /* DKC_SCRYPT_INC_ */
