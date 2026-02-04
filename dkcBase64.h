/*!
@file dkcBase64.h
@brief Base64 エンコード/デコード (RFC 4648)
@note
標準Base64 (A-Za-z0-9+/, パディング '=') の one-shot API。
*/

#ifndef DKUTIL_C_BASE64_H
#define DKUTIL_C_BASE64_H

#include "dkcOSIndependent.h"

/*!
@param src_size[in] 元データのサイズ
@return エンコード後に必要なバッファサイズ (null終端込み)
*/
DKC_EXTERN size_t WINAPI dkcBase64EncodeSize(size_t src_size);

/*!
@param src[in] Base64文字列
@param src_len[in] Base64文字列の長さ
@return デコード後のデータサイズ (概算、パディング考慮)
*/
DKC_EXTERN size_t WINAPI dkcBase64DecodeSize(const char *src,size_t src_len);

/*!
@param dest[out] エンコード結果を格納するバッファ (null終端される)
@param dest_size[in] destバッファのサイズ
@param src[in] 元データ
@param src_size[in] 元データのサイズ
@param out_len[out] 書き込まれた文字数 (null終端を含まない)。NULLでも可。
@return edk_SUCCEEDED / edk_ArgumentException / edk_BufferOverFlow
*/
DKC_EXTERN int WINAPI dkcBase64Encode(
	char *dest,size_t dest_size,
	const BYTE *src,size_t src_size,
	size_t *out_len);

/*!
@param dest[out] デコード結果を格納するバッファ
@param dest_size[in] destバッファのサイズ
@param src[in] Base64文字列
@param src_len[in] Base64文字列の長さ
@param out_len[out] デコードされたバイト数。NULLでも可。
@return edk_SUCCEEDED / edk_ArgumentException / edk_BufferOverFlow / edk_FAILED
*/
DKC_EXTERN int WINAPI dkcBase64Decode(
	BYTE *dest,size_t dest_size,
	const char *src,size_t src_len,
	size_t *out_len);

#endif /* DKUTIL_C_BASE64_H */
