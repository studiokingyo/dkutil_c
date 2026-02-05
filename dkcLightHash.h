/*! @file
	@brief 非暗号学的軽量ハッシュ関数ライブラリ
	@author d.Kingyo
	@note
	高速なハッシュテーブル、チェックサム、データ指紋用の
	非暗号学的ハッシュ関数コレクション

	収録アルゴリズム:
	【クラシック】
	- DJB2          : Dan Bernstein作、超シンプル
	- SDBM          : SDBMデータベース用
	- FNV-1/FNV-1a  : Fowler-Noll-Vo、汎用
	- Jenkins OAAT  : One-at-a-time
	- ELF Hash      : Unix ELFシンボルテーブル用
	- PJW Hash      : Peter J. Weinberger
	- SuperFastHash : Paul Hsieh

	【高速モダン】
	- MurmurHash2   : Austin Appleby (32/64bit)
	- MurmurHash3   : Austin Appleby (32/128bit)
	- xxHash32/64   : Yann Collet、超高速
	- CityHash64    : Google
	- wyhash        : Wang Yi、最速級
	- FxHash        : Firefox/Rust、シンプル高速

	【特殊】
	- Lookup3       : Bob Jenkins
	- Pearson       : 8bitハッシュ
*/

#ifndef DKC_LIGHTHASH_INC_
#define DKC_LIGHTHASH_INC_

#include "dkcOSIndependent.h"

#ifdef __cplusplus
extern "C" {
#endif

/* ====================================================================
 * クラシックハッシュ関数
 * ==================================================================== */

/*! @brief DJB2ハッシュ (Dan Bernstein)
	@param data データ
	@param len  データ長
	@return 32bitハッシュ値
	@note hash = hash * 33 + c
*/
DKC_EXTERN uint32 WINAPI dkcHashDJB2(const void *data, size_t len);

/*! @brief DJB2aハッシュ (XOR版)
	@note hash = hash * 33 ^ c
*/
DKC_EXTERN uint32 WINAPI dkcHashDJB2a(const void *data, size_t len);

/*! @brief SDBMハッシュ
	@note hash = hash * 65599 + c
*/
DKC_EXTERN uint32 WINAPI dkcHashSDBM(const void *data, size_t len);

/*! @brief FNV-1ハッシュ (32bit)
	@note hash = (hash * FNV_prime) ^ c
*/
DKC_EXTERN uint32 WINAPI dkcHashFNV1_32(const void *data, size_t len);

/*! @brief FNV-1aハッシュ (32bit) - 推奨
	@note hash = (hash ^ c) * FNV_prime
*/
DKC_EXTERN uint32 WINAPI dkcHashFNV1a_32(const void *data, size_t len);

/*! @brief FNV-1ハッシュ (64bit) */
DKC_EXTERN uint64 WINAPI dkcHashFNV1_64(const void *data, size_t len);

/*! @brief FNV-1aハッシュ (64bit) - 推奨 */
DKC_EXTERN uint64 WINAPI dkcHashFNV1a_64(const void *data, size_t len);

/*! @brief Jenkins One-at-a-timeハッシュ */
DKC_EXTERN uint32 WINAPI dkcHashJenkinsOAAT(const void *data, size_t len);

/*! @brief ELFハッシュ (Unix ELF形式) */
DKC_EXTERN uint32 WINAPI dkcHashELF(const void *data, size_t len);

/*! @brief PJWハッシュ (Peter J. Weinberger) */
DKC_EXTERN uint32 WINAPI dkcHashPJW(const void *data, size_t len);

/*! @brief SuperFastHash (Paul Hsieh) */
DKC_EXTERN uint32 WINAPI dkcHashSuperFast(const void *data, size_t len);

/*! @brief Pearsonハッシュ (8bit)
	@return 8bitハッシュ値
*/
DKC_EXTERN uint8 WINAPI dkcHashPearson(const void *data, size_t len);

/* ====================================================================
 * 高速モダンハッシュ関数
 * ==================================================================== */

/*! @brief MurmurHash2 (32bit) */
DKC_EXTERN uint32 WINAPI dkcHashMurmur2_32(const void *data, size_t len, uint32 seed);

/*! @brief MurmurHash2 (64bit) */
DKC_EXTERN uint64 WINAPI dkcHashMurmur2_64(const void *data, size_t len, uint64 seed);

/*! @brief MurmurHash3 (32bit) */
DKC_EXTERN uint32 WINAPI dkcHashMurmur3_32(const void *data, size_t len, uint32 seed);

/*! @brief MurmurHash3 (128bit)
	@param out 128bit出力 (uint64[2])
*/
DKC_EXTERN void WINAPI dkcHashMurmur3_128(const void *data, size_t len, uint32 seed, uint64 *out);

/*! @brief xxHash32 */
DKC_EXTERN uint32 WINAPI dkcHashXX32(const void *data, size_t len, uint32 seed);

/*! @brief xxHash64 */
DKC_EXTERN uint64 WINAPI dkcHashXX64(const void *data, size_t len, uint64 seed);

/*! @brief CityHash64 (Google) */
DKC_EXTERN uint64 WINAPI dkcHashCity64(const void *data, size_t len);

/*! @brief CityHash64 with seed */
DKC_EXTERN uint64 WINAPI dkcHashCity64WithSeed(const void *data, size_t len, uint64 seed);

/*! @brief wyhash (Wang Yi) - 最速級 */
DKC_EXTERN uint64 WINAPI dkcHashWy(const void *data, size_t len, uint64 seed);

/*! @brief FxHash (Firefox/Rust) - 32bit */
DKC_EXTERN uint32 WINAPI dkcHashFx32(const void *data, size_t len);

/*! @brief FxHash (Firefox/Rust) - 64bit */
DKC_EXTERN uint64 WINAPI dkcHashFx64(const void *data, size_t len);

/* ====================================================================
 * Jenkins Lookup3
 * ==================================================================== */

/*! @brief Jenkins lookup3 - 単一32bitハッシュ */
DKC_EXTERN uint32 WINAPI dkcHashLookup3(const void *data, size_t len, uint32 seed);

/*! @brief Jenkins lookup3 - 2つの32bitハッシュを生成
	@param pc [in/out] 初期値/出力ハッシュ1
	@param pb [in/out] 初期値/出力ハッシュ2
*/
DKC_EXTERN void WINAPI dkcHashLookup3Pair(const void *data, size_t len,
	uint32 *pc, uint32 *pb);

/* ====================================================================
 * ユーティリティ
 * ==================================================================== */

/*! @brief 文字列用ハッシュ (FNV-1aベース) */
DKC_EXTERN uint32 WINAPI dkcHashString(const char *str);

/*! @brief 整数用ハッシュ (32bit) */
DKC_EXTERN uint32 WINAPI dkcHashInt32(uint32 x);

/*! @brief 整数用ハッシュ (64bit) */
DKC_EXTERN uint64 WINAPI dkcHashInt64(uint64 x);

/*! @brief ポインタ用ハッシュ */
DKC_EXTERN size_t WINAPI dkcHashPtr(const void *ptr);

/*! @brief ハッシュ値の結合 (複合キー用)
	@param h1 ハッシュ値1
	@param h2 ハッシュ値2
	@return 結合されたハッシュ値
*/
DKC_EXTERN uint32 WINAPI dkcHashCombine32(uint32 h1, uint32 h2);
DKC_EXTERN uint64 WINAPI dkcHashCombine64(uint64 h1, uint64 h2);

#ifdef __cplusplus
}
#endif

#endif /* DKC_LIGHTHASH_INC_ */
