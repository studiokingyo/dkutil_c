/*! @file
	@brief bcrypt パスワードハッシュ
	@author d.Kingyo
	@note
	bcryptはProvos & Mazieres (1999)が設計したパスワードハッシュ関数で、
	Blowfishベースのキー拡張を利用します。

	特徴:
	- コスト調整可能（2^cost回のキー拡張）
	- ソルト付き
	- 出力: $2b$CC$<22char-salt><31char-hash>

	参考: Provos & Mazieres, "A Future-Adaptable Password Scheme", 1999
*/

#ifndef DKC_BCRYPT_INC_
#define DKC_BCRYPT_INC_

#include "dkcOSIndependent.h"

#ifdef __cplusplus
extern "C" {
#endif

/* ====================================================================
 * 定数定義
 * ==================================================================== */

/*! @brief ハッシュ文字列出力サイズ */
#define dkcd_BCRYPT_HASH_SIZE       60

/*! @brief ソルトサイズ（バイト） */
#define dkcd_BCRYPT_SALT_SIZE       16

/*! @brief 最小コスト */
#define dkcd_BCRYPT_MIN_COST        4

/*! @brief 最大コスト */
#define dkcd_BCRYPT_MAX_COST        31

/*! @brief デフォルトコスト */
#define dkcd_BCRYPT_DEFAULT_COST    12

/* ====================================================================
 * 関数宣言
 * ==================================================================== */

/*!
	@brief パスワードをbcryptハッシュする
	@param password パスワード
	@param pwdlen パスワード長
	@param salt ソルト（16バイト）
	@param cost コスト（4-31）
	@param output 出力バッファ（60バイト以上）
	@param outsize 出力バッファサイズ
	@return edk_SUCCEEDED:成功 edk_FAILED:失敗
*/
DKC_EXTERN int WINAPI dkcBcryptHash(
	const unsigned char *password, size_t pwdlen,
	const unsigned char *salt, int cost,
	char *output, size_t outsize);

/*!
	@brief bcryptハッシュを検証する
	@param password パスワード
	@param pwdlen パスワード長
	@param hash_str bcryptハッシュ文字列（$2b$...）
	@return edk_SUCCEEDED:一致 edk_FAILED:不一致
*/
DKC_EXTERN int WINAPI dkcBcryptVerify(
	const unsigned char *password, size_t pwdlen,
	const char *hash_str);

#ifdef __cplusplus
}
#endif

#endif /* DKC_BCRYPT_INC_ */
