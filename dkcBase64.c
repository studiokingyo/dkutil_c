/*!
@file dkcBase64.c
@brief Base64 エンコード/デコード実装 (RFC 4648)
*/
#define DKUTIL_C_BASE64_C
#include "dkcBase64.h"

static const char s_base64_encode_table[64] = {
	'A','B','C','D','E','F','G','H','I','J','K','L','M',
	'N','O','P','Q','R','S','T','U','V','W','X','Y','Z',
	'a','b','c','d','e','f','g','h','i','j','k','l','m',
	'n','o','p','q','r','s','t','u','v','w','x','y','z',
	'0','1','2','3','4','5','6','7','8','9','+','/'
};

/* decode LUT: 0-63 = valid, 0xFF = invalid, 0xFE = padding ('=') */
static const BYTE s_base64_decode_table[256] = {
	0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
	0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
	0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,  62,0xFF,0xFF,0xFF,  63,
	  52,  53,  54,  55,  56,  57,  58,  59,  60,  61,0xFF,0xFF,0xFF,0xFE,0xFF,0xFF,
	0xFF,   0,   1,   2,   3,   4,   5,   6,   7,   8,   9,  10,  11,  12,  13,  14,
	  15,  16,  17,  18,  19,  20,  21,  22,  23,  24,  25,0xFF,0xFF,0xFF,0xFF,0xFF,
	0xFF,  26,  27,  28,  29,  30,  31,  32,  33,  34,  35,  36,  37,  38,  39,  40,
	  41,  42,  43,  44,  45,  46,  47,  48,  49,  50,  51,0xFF,0xFF,0xFF,0xFF,0xFF,
	0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
	0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
	0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
	0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
	0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
	0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
	0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
	0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF
};

size_t WINAPI dkcBase64EncodeSize(size_t src_size)
{
	/* ((src_size + 2) / 3) * 4 + 1 for null terminator */
	return ((src_size + 2) / 3) * 4 + 1;
}

size_t WINAPI dkcBase64DecodeSize(const char *src,size_t src_len)
{
	size_t padding = 0;
	if(NULL==src || 0==src_len) return 0;

	if(src_len >= 1 && src[src_len - 1] == '=') padding++;
	if(src_len >= 2 && src[src_len - 2] == '=') padding++;

	return (src_len / 4) * 3 - padding;
}

int WINAPI dkcBase64Encode(
	char *dest,size_t dest_size,
	const BYTE *src,size_t src_size,
	size_t *out_len)
{
	size_t needed;
	size_t i;
	size_t di;
	DWORD triple;
	size_t remaining;

	if(NULL==dest) return edk_ArgumentException;

	/* empty input => empty output */
	if(0==src_size || NULL==src){
		if(dest_size < 1) return edk_BufferOverFlow;
		dest[0] = '\0';
		if(out_len) *out_len = 0;
		return edk_SUCCEEDED;
	}

	needed = dkcBase64EncodeSize(src_size);
	if(dest_size < needed) return edk_BufferOverFlow;

	di = 0;
	/* process full 3-byte groups */
	i = 0;
	while(i + 3 <= src_size){
		triple = ((DWORD)src[i] << 16) | ((DWORD)src[i+1] << 8) | (DWORD)src[i+2];
		dest[di++] = s_base64_encode_table[(triple >> 18) & 0x3F];
		dest[di++] = s_base64_encode_table[(triple >> 12) & 0x3F];
		dest[di++] = s_base64_encode_table[(triple >>  6) & 0x3F];
		dest[di++] = s_base64_encode_table[ triple        & 0x3F];
		i += 3;
	}

	/* handle remaining bytes */
	remaining = src_size - i;
	if(remaining == 1){
		triple = (DWORD)src[i] << 16;
		dest[di++] = s_base64_encode_table[(triple >> 18) & 0x3F];
		dest[di++] = s_base64_encode_table[(triple >> 12) & 0x3F];
		dest[di++] = '=';
		dest[di++] = '=';
	}else if(remaining == 2){
		triple = ((DWORD)src[i] << 16) | ((DWORD)src[i+1] << 8);
		dest[di++] = s_base64_encode_table[(triple >> 18) & 0x3F];
		dest[di++] = s_base64_encode_table[(triple >> 12) & 0x3F];
		dest[di++] = s_base64_encode_table[(triple >>  6) & 0x3F];
		dest[di++] = '=';
	}

	dest[di] = '\0';
	if(out_len) *out_len = di;

	return edk_SUCCEEDED;
}

int WINAPI dkcBase64Decode(
	BYTE *dest,size_t dest_size,
	const char *src,size_t src_len,
	size_t *out_len)
{
	size_t needed;
	size_t i;
	size_t di;
	BYTE a,b,c,d;
	DWORD triple;

	if(NULL==dest) return edk_ArgumentException;

	/* empty input => empty output */
	if(0==src_len || NULL==src){
		if(out_len) *out_len = 0;
		return edk_SUCCEEDED;
	}

	/* src_len must be multiple of 4 */
	if(src_len % 4 != 0) return edk_FAILED;

	needed = dkcBase64DecodeSize(src,src_len);
	if(dest_size < needed) return edk_BufferOverFlow;

	di = 0;
	for(i = 0; i < src_len; i += 4){
		a = s_base64_decode_table[(BYTE)src[i]];
		b = s_base64_decode_table[(BYTE)src[i+1]];
		c = s_base64_decode_table[(BYTE)src[i+2]];
		d = s_base64_decode_table[(BYTE)src[i+3]];

		/* check for invalid characters (but allow padding 0xFE) */
		if(a == 0xFF || b == 0xFF) return edk_FAILED;
		if(c == 0xFF || d == 0xFF) return edk_FAILED;

		/* treat padding as 0 for computation */
		triple = ((DWORD)(a == 0xFE ? 0 : a) << 18)
		       | ((DWORD)(b == 0xFE ? 0 : b) << 12)
		       | ((DWORD)(c == 0xFE ? 0 : c) <<  6)
		       | ((DWORD)(d == 0xFE ? 0 : d));

		dest[di++] = (BYTE)((triple >> 16) & 0xFF);
		if(c != 0xFE){
			dest[di++] = (BYTE)((triple >> 8) & 0xFF);
		}
		if(d != 0xFE){
			dest[di++] = (BYTE)(triple & 0xFF);
		}
	}

	if(out_len) *out_len = di;
	return edk_SUCCEEDED;
}
