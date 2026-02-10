/*! @file
	@brief 多倍長整数演算 (BigInteger)
	@author d.Kingyo
	@note
	任意精度の整数演算を提供する。
	32bitリム (uint32) を配列で保持し、リトルエンディアン (digits[0]が最下位)。

	参考: TAOCP Vol.2 §4.3 準数値算法
	      Knuth Algorithm D (除算)

	特徴:
	- 四則演算: 加減乗除
	- ビット演算: シフト
	- 数論: GCD, べき乗剰余 (ModPow)
	- 文字列変換: 10進/16進
*/

#ifndef DKC_BIGINTEGER_INC_
#define DKC_BIGINTEGER_INC_

#include "dkcOSIndependent.h"

#ifdef __cplusplus
extern "C" {
#endif

/* ====================================================================
 * 型定義
 * ==================================================================== */

/*! @brief 多倍長整数構造体 */
typedef struct dkc_BigInteger {
	uint32 *digits;      /*!< リム配列 (リトルエンディアン: digits[0]が最下位) */
	size_t used;         /*!< 使用中のリム数 */
	size_t alloc;        /*!< 確保済みリム数 */
	int sign;            /*!< 1:正, -1:負, 0:ゼロ */
} DKC_BIGINTEGER;

/* ====================================================================
 * 生成・解放
 * ==================================================================== */

/*!
	@brief BigIntegerを生成する
	@param bi 構造体へのポインタ
	@param initial_alloc 初期リム数 (0でデフォルト)
	@return edk_SUCCEEDED:成功 edk_FAILED:失敗
*/
DKC_EXTERN int WINAPI dkcBigIntCreate(DKC_BIGINTEGER *bi, size_t initial_alloc);

/*!
	@brief BigIntegerを解放する
	@param bi 構造体へのポインタ
*/
DKC_EXTERN void WINAPI dkcBigIntFree(DKC_BIGINTEGER *bi);

/*!
	@brief BigIntegerをコピーする
	@param dest コピー先
	@param src コピー元
	@return edk_SUCCEEDED:成功 edk_FAILED:失敗
*/
DKC_EXTERN int WINAPI dkcBigIntCopy(DKC_BIGINTEGER *dest, const DKC_BIGINTEGER *src);

/* ====================================================================
 * 初期化
 * ==================================================================== */

/*!
	@brief int値からBigIntegerを設定する
	@param bi 構造体へのポインタ
	@param value 設定する値
	@return edk_SUCCEEDED:成功 edk_FAILED:失敗
*/
DKC_EXTERN int WINAPI dkcBigIntFromInt(DKC_BIGINTEGER *bi, long value);

/*!
	@brief 文字列からBigIntegerを設定する
	@param bi 構造体へのポインタ
	@param str 文字列 ("123", "-456", "0xFF")
	@param base 基数 (10 or 16)
	@return edk_SUCCEEDED:成功 edk_FAILED:失敗
*/
DKC_EXTERN int WINAPI dkcBigIntFromString(DKC_BIGINTEGER *bi, const char *str, int base);

/*!
	@brief BigIntegerを文字列に変換する
	@param bi 構造体へのポインタ
	@param buf 出力バッファ
	@param buf_size バッファサイズ
	@param base 基数 (10 or 16)
	@return edk_SUCCEEDED:成功 edk_FAILED:失敗
*/
DKC_EXTERN int WINAPI dkcBigIntToString(const DKC_BIGINTEGER *bi, char *buf, size_t buf_size, int base);

/* ====================================================================
 * 四則演算
 * ==================================================================== */

/*!
	@brief 加算: result = a + b
*/
DKC_EXTERN int WINAPI dkcBigIntAdd(DKC_BIGINTEGER *result,
	const DKC_BIGINTEGER *a, const DKC_BIGINTEGER *b);

/*!
	@brief 減算: result = a - b
*/
DKC_EXTERN int WINAPI dkcBigIntSub(DKC_BIGINTEGER *result,
	const DKC_BIGINTEGER *a, const DKC_BIGINTEGER *b);

/*!
	@brief 乗算: result = a * b
*/
DKC_EXTERN int WINAPI dkcBigIntMul(DKC_BIGINTEGER *result,
	const DKC_BIGINTEGER *a, const DKC_BIGINTEGER *b);

/*!
	@brief 除算: quotient = a / b, remainder = a % b
	@param quotient 商 (NULLでも可)
	@param remainder 剰余 (NULLでも可)
*/
DKC_EXTERN int WINAPI dkcBigIntDiv(DKC_BIGINTEGER *quotient, DKC_BIGINTEGER *remainder,
	const DKC_BIGINTEGER *a, const DKC_BIGINTEGER *b);

/*!
	@brief 剰余: result = a % b
*/
DKC_EXTERN int WINAPI dkcBigIntMod(DKC_BIGINTEGER *result,
	const DKC_BIGINTEGER *a, const DKC_BIGINTEGER *b);

/* ====================================================================
 * 比較
 * ==================================================================== */

/*!
	@brief 比較
	@return -1: a < b, 0: a == b, 1: a > b
*/
DKC_EXTERN int WINAPI dkcBigIntCompare(const DKC_BIGINTEGER *a, const DKC_BIGINTEGER *b);

/*!
	@brief ゼロかどうか確認する
	@return TRUE:ゼロ FALSE:ゼロでない
*/
DKC_EXTERN BOOL WINAPI dkcBigIntIsZero(const DKC_BIGINTEGER *bi);

/* ====================================================================
 * ビット演算
 * ==================================================================== */

/*!
	@brief 左シフト: result = a << shift
*/
DKC_EXTERN int WINAPI dkcBigIntShiftLeft(DKC_BIGINTEGER *result,
	const DKC_BIGINTEGER *a, size_t shift);

/*!
	@brief 右シフト: result = a >> shift
*/
DKC_EXTERN int WINAPI dkcBigIntShiftRight(DKC_BIGINTEGER *result,
	const DKC_BIGINTEGER *a, size_t shift);

/* ====================================================================
 * 数論
 * ==================================================================== */

/*!
	@brief 最大公約数: result = gcd(a, b)
*/
DKC_EXTERN int WINAPI dkcBigIntGCD(DKC_BIGINTEGER *result,
	const DKC_BIGINTEGER *a, const DKC_BIGINTEGER *b);

/*!
	@brief べき乗剰余: result = base^exp mod mod
*/
DKC_EXTERN int WINAPI dkcBigIntModPow(DKC_BIGINTEGER *result,
	const DKC_BIGINTEGER *base, const DKC_BIGINTEGER *exp,
	const DKC_BIGINTEGER *mod);

/* ====================================================================
 * 情報
 * ==================================================================== */

/*!
	@brief ビット数を取得する
	@return ビット数 (0の場合は0)
*/
DKC_EXTERN size_t WINAPI dkcBigIntBitCount(const DKC_BIGINTEGER *bi);

#ifdef __cplusplus
}
#endif

#endif /* DKC_BIGINTEGER_INC_ */
