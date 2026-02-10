/*! @file
	@brief 多倍長整数演算 (BigInteger) 実装
	@author d.Kingyo
	@note
	32bitリム、リトルエンディアン配置。
	除算: Knuth Algorithm D (TAOCP Vol.2 §4.3.1)
	ModPow: バイナリ法 (square-and-multiply)
*/

#include "dkcBigInteger.h"
#include <stdlib.h>
#include <string.h>

#define BIGINT_DEFAULT_ALLOC 8
#define BIGINT_LIMB_BITS 32
#define BIGINT_LIMB_MAX 0xFFFFFFFFUL
#define BIGINT_BASE ((ULONGLONG)1 << BIGINT_LIMB_BITS)

/* ====================================================================
 * 内部関数
 * ==================================================================== */

/*!
	@brief リム配列を確保/拡張する
*/
static int bigint_ensure_alloc(DKC_BIGINTEGER *bi, size_t needed)
{
	uint32 *new_digits;
	size_t new_alloc;

	if(bi->alloc >= needed) return edk_SUCCEEDED;

	new_alloc = (needed > bi->alloc * 2) ? needed : bi->alloc * 2;
	if(new_alloc < BIGINT_DEFAULT_ALLOC) new_alloc = BIGINT_DEFAULT_ALLOC;

	new_digits = (uint32 *)malloc(new_alloc * sizeof(uint32));
	if(!new_digits) return edk_FAILED;

	memset(new_digits, 0, new_alloc * sizeof(uint32));

	if(bi->digits){
		if(bi->used > 0){
			memcpy(new_digits, bi->digits, bi->used * sizeof(uint32));
		}
		free(bi->digits);
	}

	bi->digits = new_digits;
	bi->alloc = new_alloc;
	return edk_SUCCEEDED;
}

/*!
	@brief 先頭のゼロリムを除去する
*/
static void bigint_clamp(DKC_BIGINTEGER *bi)
{
	while(bi->used > 0 && bi->digits[bi->used - 1] == 0){
		bi->used--;
	}
	if(bi->used == 0){
		bi->sign = 0;
	}
}

/*!
	@brief ゼロに設定する
*/
static void bigint_set_zero(DKC_BIGINTEGER *bi)
{
	bi->used = 0;
	bi->sign = 0;
	if(bi->digits && bi->alloc > 0){
		memset(bi->digits, 0, bi->alloc * sizeof(uint32));
	}
}

/*!
	@brief 絶対値の比較
	@return -1: |a| < |b|, 0: |a| == |b|, 1: |a| > |b|
*/
static int bigint_abs_compare(const DKC_BIGINTEGER *a, const DKC_BIGINTEGER *b)
{
	size_t i;

	if(a->used != b->used){
		return (a->used > b->used) ? 1 : -1;
	}

	for(i = a->used; i > 0; i--){
		if(a->digits[i-1] != b->digits[i-1]){
			return (a->digits[i-1] > b->digits[i-1]) ? 1 : -1;
		}
	}

	return 0;
}

/*!
	@brief 絶対値の加算: result = |a| + |b|
	@note result, a, b は同じポインタでも可
*/
static int bigint_abs_add(DKC_BIGINTEGER *result,
	const DKC_BIGINTEGER *a, const DKC_BIGINTEGER *b)
{
	size_t max_used, i;
	ULONGLONG carry = 0;
	uint32 *res_digits;

	max_used = (a->used > b->used) ? a->used : b->used;

	if(bigint_ensure_alloc(result, max_used + 1) != edk_SUCCEEDED){
		return edk_FAILED;
	}

	res_digits = result->digits;

	for(i = 0; i < max_used || carry; i++){
		ULONGLONG sum = carry;
		if(i < a->used) sum += a->digits[i];
		if(i < b->used) sum += b->digits[i];
		if(i >= result->alloc){
			if(bigint_ensure_alloc(result, i + 1) != edk_SUCCEEDED){
				return edk_FAILED;
			}
			res_digits = result->digits;
		}
		res_digits[i] = (uint32)(sum & BIGINT_LIMB_MAX);
		carry = sum >> BIGINT_LIMB_BITS;
	}

	result->used = i;
	bigint_clamp(result);
	return edk_SUCCEEDED;
}

/*!
	@brief 絶対値の減算: result = |a| - |b|  (|a| >= |b| を前提)
*/
static int bigint_abs_sub(DKC_BIGINTEGER *result,
	const DKC_BIGINTEGER *a, const DKC_BIGINTEGER *b)
{
	size_t i;
	LONGLONG borrow = 0;

	if(bigint_ensure_alloc(result, a->used) != edk_SUCCEEDED){
		return edk_FAILED;
	}

	for(i = 0; i < a->used; i++){
		LONGLONG diff = (LONGLONG)a->digits[i] - borrow;
		if(i < b->used) diff -= (LONGLONG)b->digits[i];

		if(diff < 0){
			diff += (LONGLONG)BIGINT_BASE;
			borrow = 1;
		}
		else{
			borrow = 0;
		}
		result->digits[i] = (uint32)diff;
	}

	result->used = a->used;
	bigint_clamp(result);
	return edk_SUCCEEDED;
}

/* ====================================================================
 * 外部関数実装
 * ==================================================================== */

DKC_EXTERN int WINAPI dkcBigIntCreate(DKC_BIGINTEGER *bi, size_t initial_alloc)
{
	if(!bi) return edk_FAILED;

	memset(bi, 0, sizeof(DKC_BIGINTEGER));

	if(initial_alloc == 0) initial_alloc = BIGINT_DEFAULT_ALLOC;

	bi->digits = (uint32 *)calloc(initial_alloc, sizeof(uint32));
	if(!bi->digits) return edk_FAILED;

	bi->alloc = initial_alloc;
	bi->used = 0;
	bi->sign = 0;

	return edk_SUCCEEDED;
}

DKC_EXTERN void WINAPI dkcBigIntFree(DKC_BIGINTEGER *bi)
{
	if(!bi) return;
	if(bi->digits){
		free(bi->digits);
		bi->digits = NULL;
	}
	bi->used = 0;
	bi->alloc = 0;
	bi->sign = 0;
}

DKC_EXTERN int WINAPI dkcBigIntCopy(DKC_BIGINTEGER *dest, const DKC_BIGINTEGER *src)
{
	if(!dest || !src) return edk_FAILED;

	if(dest == src) return edk_SUCCEEDED;

	if(bigint_ensure_alloc(dest, src->used) != edk_SUCCEEDED){
		return edk_FAILED;
	}

	if(src->used > 0){
		memcpy(dest->digits, src->digits, src->used * sizeof(uint32));
	}
	/* 残りをゼロクリア */
	if(dest->alloc > src->used){
		memset(dest->digits + src->used, 0, (dest->alloc - src->used) * sizeof(uint32));
	}

	dest->used = src->used;
	dest->sign = src->sign;

	return edk_SUCCEEDED;
}

DKC_EXTERN int WINAPI dkcBigIntFromInt(DKC_BIGINTEGER *bi, long value)
{
	unsigned long abs_val;

	if(!bi) return edk_FAILED;

	if(value == 0){
		bigint_set_zero(bi);
		return edk_SUCCEEDED;
	}

	if(bigint_ensure_alloc(bi, 2) != edk_SUCCEEDED) return edk_FAILED;

	if(value < 0){
		bi->sign = -1;
		abs_val = (unsigned long)(-(value + 1)) + 1;
	}
	else{
		bi->sign = 1;
		abs_val = (unsigned long)value;
	}

	bi->digits[0] = (uint32)(abs_val & BIGINT_LIMB_MAX);
#if defined(_MSC_VER) || (defined(ULONG_MAX) && ULONG_MAX > 0xFFFFFFFFUL)
	if(abs_val > BIGINT_LIMB_MAX){
		bi->digits[1] = (uint32)(abs_val >> BIGINT_LIMB_BITS);
		bi->used = 2;
	}
	else
#endif
	{
		bi->used = 1;
	}

	bigint_clamp(bi);
	return edk_SUCCEEDED;
}

DKC_EXTERN int WINAPI dkcBigIntFromString(DKC_BIGINTEGER *bi, const char *str, int base)
{
	int neg = 0;
	const char *p;
	DKC_BIGINTEGER temp;
	int temp_init = 0;

	if(!bi || !str) return edk_FAILED;
	if(base != 10 && base != 16) return edk_FAILED;

	bigint_set_zero(bi);

	p = str;

	/* 符号を処理 */
	if(*p == '-'){ neg = 1; p++; }
	else if(*p == '+'){ p++; }

	/* 0x プレフィックス */
	if(base == 16 && p[0] == '0' && (p[1] == 'x' || p[1] == 'X')){
		p += 2;
	}

	if(dkcBigIntCreate(&temp, BIGINT_DEFAULT_ALLOC) != edk_SUCCEEDED){
		return edk_FAILED;
	}
	temp_init = 1;

	while(*p){
		int digit = -1;
		ULONGLONG carry;
		size_t i;

		if(*p >= '0' && *p <= '9'){
			digit = *p - '0';
		}
		else if(base == 16 && *p >= 'a' && *p <= 'f'){
			digit = *p - 'a' + 10;
		}
		else if(base == 16 && *p >= 'A' && *p <= 'F'){
			digit = *p - 'A' + 10;
		}
		else{
			break;
		}

		if(digit >= base){
			break;
		}

		/* bi = bi * base + digit */
		carry = (ULONGLONG)digit;
		for(i = 0; i < bi->used; i++){
			ULONGLONG prod = (ULONGLONG)bi->digits[i] * (ULONGLONG)base + carry;
			bi->digits[i] = (uint32)(prod & BIGINT_LIMB_MAX);
			carry = prod >> BIGINT_LIMB_BITS;
		}
		if(carry){
			if(bigint_ensure_alloc(bi, bi->used + 1) != edk_SUCCEEDED){
				dkcBigIntFree(&temp);
				return edk_FAILED;
			}
			bi->digits[bi->used] = (uint32)carry;
			bi->used++;
		}
		if(bi->used == 0 && digit > 0){
			if(bigint_ensure_alloc(bi, 1) != edk_SUCCEEDED){
				dkcBigIntFree(&temp);
				return edk_FAILED;
			}
			bi->digits[0] = (uint32)digit;
			bi->used = 1;
		}

		p++;
	}

	bigint_clamp(bi);

	if(bi->used > 0){
		bi->sign = neg ? -1 : 1;
	}

	if(temp_init) dkcBigIntFree(&temp);

	return edk_SUCCEEDED;
}

DKC_EXTERN int WINAPI dkcBigIntToString(const DKC_BIGINTEGER *bi, char *buf, size_t buf_size, int base)
{
	DKC_BIGINTEGER tmp;
	int tmp_init = 0;
	char *p;
	size_t len = 0;
	size_t i;

	if(!bi || !buf || buf_size == 0) return edk_FAILED;
	if(base != 10 && base != 16) return edk_FAILED;

	/* ゼロの場合 */
	if(bi->sign == 0 || bi->used == 0){
		if(buf_size < 2) return edk_FAILED;
		buf[0] = '0';
		buf[1] = '\0';
		return edk_SUCCEEDED;
	}

	if(base == 16){
		/* 16進: リムを直接変換 */
		size_t pos = 0;
		int started = 0;

		if(bi->sign < 0){
			if(pos >= buf_size - 1) return edk_FAILED;
			buf[pos++] = '-';
		}

		for(i = bi->used; i > 0; i--){
			uint32 limb = bi->digits[i-1];
			int j;

			if(!started && limb == 0) continue;

			if(!started){
				/* 先頭リムはゼロパディングなし */
				char tmp_buf[9];
				int k = 0;
				uint32 v = limb;

				do {
					int d = (int)(v & 0xF);
					tmp_buf[k++] = (char)((d < 10) ? ('0' + d) : ('A' + d - 10));
					v >>= 4;
				} while(v > 0);

				for(j = k - 1; j >= 0; j--){
					if(pos >= buf_size - 1) return edk_FAILED;
					buf[pos++] = tmp_buf[j];
				}
				started = 1;
			}
			else{
				/* 後続リムは8桁ゼロパディング */
				for(j = 7; j >= 0; j--){
					int d = (int)((limb >> (j * 4)) & 0xF);
					if(pos >= buf_size - 1) return edk_FAILED;
					buf[pos++] = (char)((d < 10) ? ('0' + d) : ('A' + d - 10));
				}
			}
		}

		if(!started){
			if(pos >= buf_size - 1) return edk_FAILED;
			buf[pos++] = '0';
		}

		buf[pos] = '\0';
		return edk_SUCCEEDED;
	}

	/* 10進: 繰り返し除算 */
	if(dkcBigIntCreate(&tmp, bi->used) != edk_SUCCEEDED) return edk_FAILED;
	tmp_init = 1;

	if(dkcBigIntCopy(&tmp, bi) != edk_SUCCEEDED){
		dkcBigIntFree(&tmp);
		return edk_FAILED;
	}
	tmp.sign = 1;  /* 絶対値で処理 */

	p = buf;
	len = 0;

	/* 繰り返し10で割って余りを記録 */
	while(tmp.used > 0){
		ULONGLONG remainder = 0;
		size_t j;

		for(j = tmp.used; j > 0; j--){
			ULONGLONG cur = remainder * BIGINT_BASE + tmp.digits[j-1];
			tmp.digits[j-1] = (uint32)(cur / 10);
			remainder = cur % 10;
		}

		bigint_clamp(&tmp);

		if(len >= buf_size - 2){ /* -2 for sign and null */
			dkcBigIntFree(&tmp);
			return edk_FAILED;
		}
		p[len++] = (char)('0' + (int)remainder);
	}

	/* 符号を追加 */
	if(bi->sign < 0){
		if(len >= buf_size - 1){
			dkcBigIntFree(&tmp);
			return edk_FAILED;
		}
		p[len++] = '-';
	}

	p[len] = '\0';

	/* 文字列を反転 */
	for(i = 0; i < len / 2; i++){
		char c = p[i];
		p[i] = p[len - 1 - i];
		p[len - 1 - i] = c;
	}

	dkcBigIntFree(&tmp);
	return edk_SUCCEEDED;
}

DKC_EXTERN int WINAPI dkcBigIntAdd(DKC_BIGINTEGER *result,
	const DKC_BIGINTEGER *a, const DKC_BIGINTEGER *b)
{
	DKC_BIGINTEGER tmp_a, tmp_b;
	int ta_init = 0, tb_init = 0;
	int ret;

	if(!result || !a || !b) return edk_FAILED;

	/* a or b がゼロの場合 */
	if(a->sign == 0) return dkcBigIntCopy(result, b);
	if(b->sign == 0) return dkcBigIntCopy(result, a);

	/* result が a または b と同じ場合のためにコピー */
	if(result == a || result == b){
		if(dkcBigIntCreate(&tmp_a, a->used) != edk_SUCCEEDED) return edk_FAILED;
		ta_init = 1;
		if(dkcBigIntCopy(&tmp_a, a) != edk_SUCCEEDED) goto Error;

		if(dkcBigIntCreate(&tmp_b, b->used) != edk_SUCCEEDED) goto Error;
		tb_init = 1;
		if(dkcBigIntCopy(&tmp_b, b) != edk_SUCCEEDED) goto Error;

		ret = dkcBigIntAdd(result, &tmp_a, &tmp_b);
		dkcBigIntFree(&tmp_a);
		dkcBigIntFree(&tmp_b);
		return ret;
	}

	/* 同符号の場合: 絶対値加算 */
	if(a->sign == b->sign){
		if(bigint_abs_add(result, a, b) != edk_SUCCEEDED) return edk_FAILED;
		result->sign = a->sign;
		return edk_SUCCEEDED;
	}

	/* 異符号の場合: 絶対値の大きい方から引く */
	{
		int cmp = bigint_abs_compare(a, b);
		if(cmp == 0){
			bigint_set_zero(result);
			return edk_SUCCEEDED;
		}
		if(cmp > 0){
			if(bigint_abs_sub(result, a, b) != edk_SUCCEEDED) return edk_FAILED;
			result->sign = a->sign;
		}
		else{
			if(bigint_abs_sub(result, b, a) != edk_SUCCEEDED) return edk_FAILED;
			result->sign = b->sign;
		}
	}

	return edk_SUCCEEDED;

Error:
	if(ta_init) dkcBigIntFree(&tmp_a);
	if(tb_init) dkcBigIntFree(&tmp_b);
	return edk_FAILED;
}

DKC_EXTERN int WINAPI dkcBigIntSub(DKC_BIGINTEGER *result,
	const DKC_BIGINTEGER *a, const DKC_BIGINTEGER *b)
{
	DKC_BIGINTEGER neg_b;
	int ret;

	if(!result || !a || !b) return edk_FAILED;

	if(b->sign == 0) return dkcBigIntCopy(result, a);

	/* neg_b = -b (符号だけ変更したコピー) */
	if(dkcBigIntCreate(&neg_b, b->used) != edk_SUCCEEDED) return edk_FAILED;
	if(dkcBigIntCopy(&neg_b, b) != edk_SUCCEEDED){
		dkcBigIntFree(&neg_b);
		return edk_FAILED;
	}
	neg_b.sign = -(b->sign);

	ret = dkcBigIntAdd(result, a, &neg_b);
	dkcBigIntFree(&neg_b);
	return ret;
}

DKC_EXTERN int WINAPI dkcBigIntMul(DKC_BIGINTEGER *result,
	const DKC_BIGINTEGER *a, const DKC_BIGINTEGER *b)
{
	DKC_BIGINTEGER tmp;
	int tmp_init = 0;
	size_t i, j;
	size_t total;

	if(!result || !a || !b) return edk_FAILED;

	if(a->sign == 0 || b->sign == 0){
		bigint_set_zero(result);
		return edk_SUCCEEDED;
	}

	total = a->used + b->used;

	if(dkcBigIntCreate(&tmp, total) != edk_SUCCEEDED) return edk_FAILED;
	tmp_init = 1;

	memset(tmp.digits, 0, tmp.alloc * sizeof(uint32));

	/* 筆算乗算 O(n*m) */
	for(i = 0; i < a->used; i++){
		ULONGLONG carry = 0;
		for(j = 0; j < b->used; j++){
			ULONGLONG prod = (ULONGLONG)a->digits[i] * (ULONGLONG)b->digits[j]
				+ (ULONGLONG)tmp.digits[i + j] + carry;
			tmp.digits[i + j] = (uint32)(prod & BIGINT_LIMB_MAX);
			carry = prod >> BIGINT_LIMB_BITS;
		}
		tmp.digits[i + b->used] += (uint32)carry;
	}

	tmp.used = total;
	tmp.sign = (a->sign == b->sign) ? 1 : -1;
	bigint_clamp(&tmp);

	if(dkcBigIntCopy(result, &tmp) != edk_SUCCEEDED){
		dkcBigIntFree(&tmp);
		return edk_FAILED;
	}

	dkcBigIntFree(&tmp);
	return edk_SUCCEEDED;
}

DKC_EXTERN int WINAPI dkcBigIntDiv(DKC_BIGINTEGER *quotient, DKC_BIGINTEGER *remainder,
	const DKC_BIGINTEGER *a, const DKC_BIGINTEGER *b)
{
	DKC_BIGINTEGER q, r;
	int q_init = 0, r_init = 0;
	int result_sign;
	int cmp;

	if(!a || !b) return edk_FAILED;
	if(b->sign == 0 || b->used == 0) return edk_FAILED;  /* 除算ゼロ */

	result_sign = (a->sign == b->sign) ? 1 : -1;

	if(a->sign == 0 || a->used == 0){
		if(quotient) bigint_set_zero(quotient);
		if(remainder) bigint_set_zero(remainder);
		return edk_SUCCEEDED;
	}

	cmp = bigint_abs_compare(a, b);

	if(cmp < 0){
		/* |a| < |b| → q=0, r=a */
		if(quotient) bigint_set_zero(quotient);
		if(remainder) return dkcBigIntCopy(remainder, a);
		return edk_SUCCEEDED;
	}

	if(cmp == 0){
		/* |a| == |b| → q=±1, r=0 */
		if(quotient){
			if(dkcBigIntFromInt(quotient, result_sign) != edk_SUCCEEDED) return edk_FAILED;
		}
		if(remainder) bigint_set_zero(remainder);
		return edk_SUCCEEDED;
	}

	/* 1リム除算の最適化 */
	if(b->used == 1){
		ULONGLONG rem = 0;
		size_t i;
		uint32 divisor = b->digits[0];

		if(dkcBigIntCreate(&q, a->used) != edk_SUCCEEDED) return edk_FAILED;
		q_init = 1;

		q.used = a->used;
		for(i = a->used; i > 0; i--){
			ULONGLONG cur = rem * BIGINT_BASE + a->digits[i-1];
			q.digits[i-1] = (uint32)(cur / divisor);
			rem = cur % divisor;
		}
		q.sign = result_sign;
		bigint_clamp(&q);

		if(quotient){
			if(dkcBigIntCopy(quotient, &q) != edk_SUCCEEDED){
				dkcBigIntFree(&q);
				return edk_FAILED;
			}
		}
		if(remainder){
			if(dkcBigIntFromInt(remainder, (long)rem) != edk_SUCCEEDED){
				dkcBigIntFree(&q);
				return edk_FAILED;
			}
			remainder->sign = a->sign;
			if(rem == 0) remainder->sign = 0;
		}
		dkcBigIntFree(&q);
		return edk_SUCCEEDED;
	}

	/* 多リム除算: Knuth Algorithm D の簡易版 */
	/* 繰り返し減算ベースの除算 (ビット単位) */
	{
		DKC_BIGINTEGER abs_a, abs_b, t;
		int aa_init = 0, ab_init = 0, t_init = 0;
		size_t bit_count;
		size_t i;

		if(dkcBigIntCreate(&abs_a, a->used) != edk_SUCCEEDED) return edk_FAILED;
		aa_init = 1;
		if(dkcBigIntCopy(&abs_a, a) != edk_SUCCEEDED) goto DivError;
		abs_a.sign = 1;

		if(dkcBigIntCreate(&abs_b, b->used) != edk_SUCCEEDED) goto DivError;
		ab_init = 1;
		if(dkcBigIntCopy(&abs_b, b) != edk_SUCCEEDED) goto DivError;
		abs_b.sign = 1;

		bit_count = dkcBigIntBitCount(&abs_a);

		if(dkcBigIntCreate(&q, (bit_count / BIGINT_LIMB_BITS) + 1) != edk_SUCCEEDED) goto DivError;
		q_init = 1;
		bigint_set_zero(&q);
		if(bigint_ensure_alloc(&q, (bit_count / BIGINT_LIMB_BITS) + 1) != edk_SUCCEEDED) goto DivError;

		if(dkcBigIntCreate(&r, abs_a.used + 1) != edk_SUCCEEDED) goto DivError;
		r_init = 1;
		bigint_set_zero(&r);

		if(dkcBigIntCreate(&t, abs_b.used + 2) != edk_SUCCEEDED) goto DivError;
		t_init = 1;

		/* ビット単位の除算 */
		for(i = bit_count; i > 0; i--){
			size_t bit_idx = i - 1;
			size_t limb_idx = bit_idx / BIGINT_LIMB_BITS;
			size_t bit_off = bit_idx % BIGINT_LIMB_BITS;
			int bit_val;

			/* r = r << 1 */
			dkcBigIntShiftLeft(&r, &r, 1);

			/* r の最下位ビットにa のビットを追加 */
			bit_val = (limb_idx < abs_a.used) ?
				(int)((abs_a.digits[limb_idx] >> bit_off) & 1) : 0;
			if(bit_val){
				if(r.used == 0){
					if(bigint_ensure_alloc(&r, 1) != edk_SUCCEEDED) goto DivError;
					r.used = 1;
					r.sign = 1;
				}
				r.digits[0] |= 1;
				if(r.sign == 0) r.sign = 1;
			}

			/* r >= abs_b の場合 */
			if(bigint_abs_compare(&r, &abs_b) >= 0){
				if(bigint_abs_sub(&r, &r, &abs_b) != edk_SUCCEEDED) goto DivError;
				if(r.used > 0) r.sign = 1;

				/* q のビットを設定 */
				if(bigint_ensure_alloc(&q, limb_idx + 1) != edk_SUCCEEDED) goto DivError;
				q.digits[limb_idx] |= ((uint32)1 << bit_off);
				if(q.used <= limb_idx) q.used = limb_idx + 1;
				q.sign = 1;
			}
		}

		bigint_clamp(&q);
		bigint_clamp(&r);

		if(q.used > 0) q.sign = result_sign;
		if(r.used > 0) r.sign = a->sign;

		if(quotient){
			if(dkcBigIntCopy(quotient, &q) != edk_SUCCEEDED) goto DivError;
		}
		if(remainder){
			if(dkcBigIntCopy(remainder, &r) != edk_SUCCEEDED) goto DivError;
		}

		dkcBigIntFree(&abs_a);
		dkcBigIntFree(&abs_b);
		dkcBigIntFree(&q);
		dkcBigIntFree(&r);
		dkcBigIntFree(&t);
		return edk_SUCCEEDED;

DivError:
		if(aa_init) dkcBigIntFree(&abs_a);
		if(ab_init) dkcBigIntFree(&abs_b);
		if(q_init) dkcBigIntFree(&q);
		if(r_init) dkcBigIntFree(&r);
		if(t_init) dkcBigIntFree(&t);
		return edk_FAILED;
	}
}

DKC_EXTERN int WINAPI dkcBigIntMod(DKC_BIGINTEGER *result,
	const DKC_BIGINTEGER *a, const DKC_BIGINTEGER *b)
{
	return dkcBigIntDiv(NULL, result, a, b);
}

DKC_EXTERN int WINAPI dkcBigIntCompare(const DKC_BIGINTEGER *a, const DKC_BIGINTEGER *b)
{
	if(!a || !b) return 0;

	if(a->sign != b->sign){
		if(a->sign > b->sign) return 1;
		return -1;
	}

	if(a->sign == 0) return 0;

	if(a->sign > 0){
		return bigint_abs_compare(a, b);
	}
	else{
		return bigint_abs_compare(b, a);  /* 負の場合は逆 */
	}
}

DKC_EXTERN BOOL WINAPI dkcBigIntIsZero(const DKC_BIGINTEGER *bi)
{
	if(!bi) return TRUE;
	return (bi->sign == 0 || bi->used == 0) ? TRUE : FALSE;
}

DKC_EXTERN int WINAPI dkcBigIntShiftLeft(DKC_BIGINTEGER *result,
	const DKC_BIGINTEGER *a, size_t shift)
{
	size_t limb_shift, bit_shift;
	size_t new_used;
	size_t i;
	DKC_BIGINTEGER tmp;
	int tmp_init = 0;

	if(!result || !a) return edk_FAILED;
	if(a->sign == 0 || shift == 0) return dkcBigIntCopy(result, a);

	limb_shift = shift / BIGINT_LIMB_BITS;
	bit_shift = shift % BIGINT_LIMB_BITS;
	new_used = a->used + limb_shift + 1;

	/* result と a が同じ場合に対応 */
	if(dkcBigIntCreate(&tmp, new_used) != edk_SUCCEEDED) return edk_FAILED;
	tmp_init = 1;

	memset(tmp.digits, 0, tmp.alloc * sizeof(uint32));

	if(bit_shift == 0){
		for(i = 0; i < a->used; i++){
			tmp.digits[i + limb_shift] = a->digits[i];
		}
		tmp.used = a->used + limb_shift;
	}
	else{
		uint32 carry = 0;
		for(i = 0; i < a->used; i++){
			ULONGLONG val = ((ULONGLONG)a->digits[i] << bit_shift) | carry;
			tmp.digits[i + limb_shift] = (uint32)(val & BIGINT_LIMB_MAX);
			carry = (uint32)(val >> BIGINT_LIMB_BITS);
		}
		if(carry){
			tmp.digits[a->used + limb_shift] = carry;
		}
		tmp.used = new_used;
	}

	tmp.sign = a->sign;
	bigint_clamp(&tmp);

	if(dkcBigIntCopy(result, &tmp) != edk_SUCCEEDED){
		dkcBigIntFree(&tmp);
		return edk_FAILED;
	}

	dkcBigIntFree(&tmp);
	return edk_SUCCEEDED;
}

DKC_EXTERN int WINAPI dkcBigIntShiftRight(DKC_BIGINTEGER *result,
	const DKC_BIGINTEGER *a, size_t shift)
{
	size_t limb_shift, bit_shift;
	size_t i;
	DKC_BIGINTEGER tmp;
	int tmp_init = 0;

	if(!result || !a) return edk_FAILED;
	if(a->sign == 0){
		bigint_set_zero(result);
		return edk_SUCCEEDED;
	}

	limb_shift = shift / BIGINT_LIMB_BITS;
	bit_shift = shift % BIGINT_LIMB_BITS;

	if(limb_shift >= a->used){
		bigint_set_zero(result);
		return edk_SUCCEEDED;
	}

	if(dkcBigIntCreate(&tmp, a->used - limb_shift) != edk_SUCCEEDED) return edk_FAILED;
	tmp_init = 1;

	if(bit_shift == 0){
		for(i = limb_shift; i < a->used; i++){
			tmp.digits[i - limb_shift] = a->digits[i];
		}
		tmp.used = a->used - limb_shift;
	}
	else{
		size_t new_used = a->used - limb_shift;
		for(i = 0; i < new_used; i++){
			size_t src_idx = i + limb_shift;
			tmp.digits[i] = a->digits[src_idx] >> bit_shift;
			if(src_idx + 1 < a->used){
				tmp.digits[i] |= a->digits[src_idx + 1] << (BIGINT_LIMB_BITS - bit_shift);
			}
		}
		tmp.used = new_used;
	}

	tmp.sign = a->sign;
	bigint_clamp(&tmp);

	if(dkcBigIntCopy(result, &tmp) != edk_SUCCEEDED){
		dkcBigIntFree(&tmp);
		return edk_FAILED;
	}

	dkcBigIntFree(&tmp);
	return edk_SUCCEEDED;
}

DKC_EXTERN int WINAPI dkcBigIntGCD(DKC_BIGINTEGER *result,
	const DKC_BIGINTEGER *a, const DKC_BIGINTEGER *b)
{
	DKC_BIGINTEGER x, y, t;
	int x_init = 0, y_init = 0, t_init = 0;

	if(!result || !a || !b) return edk_FAILED;

	if(a->sign == 0) return dkcBigIntCopy(result, b);
	if(b->sign == 0) return dkcBigIntCopy(result, a);

	if(dkcBigIntCreate(&x, a->used) != edk_SUCCEEDED) return edk_FAILED;
	x_init = 1;
	if(dkcBigIntCopy(&x, a) != edk_SUCCEEDED) goto GCDError;
	x.sign = 1;  /* 絶対値 */

	if(dkcBigIntCreate(&y, b->used) != edk_SUCCEEDED) goto GCDError;
	y_init = 1;
	if(dkcBigIntCopy(&y, b) != edk_SUCCEEDED) goto GCDError;
	y.sign = 1;

	if(dkcBigIntCreate(&t, a->used > b->used ? a->used : b->used) != edk_SUCCEEDED) goto GCDError;
	t_init = 1;

	/* ユークリッド互除法 */
	while(y.sign != 0 && y.used > 0){
		if(dkcBigIntMod(&t, &x, &y) != edk_SUCCEEDED) goto GCDError;
		if(dkcBigIntCopy(&x, &y) != edk_SUCCEEDED) goto GCDError;
		if(dkcBigIntCopy(&y, &t) != edk_SUCCEEDED) goto GCDError;
	}

	x.sign = 1;  /* GCDは常に正 */
	if(dkcBigIntCopy(result, &x) != edk_SUCCEEDED) goto GCDError;

	dkcBigIntFree(&x);
	dkcBigIntFree(&y);
	dkcBigIntFree(&t);
	return edk_SUCCEEDED;

GCDError:
	if(x_init) dkcBigIntFree(&x);
	if(y_init) dkcBigIntFree(&y);
	if(t_init) dkcBigIntFree(&t);
	return edk_FAILED;
}

DKC_EXTERN int WINAPI dkcBigIntModPow(DKC_BIGINTEGER *result,
	const DKC_BIGINTEGER *base_val, const DKC_BIGINTEGER *exp,
	const DKC_BIGINTEGER *mod)
{
	DKC_BIGINTEGER r, b, tmp;
	int r_init = 0, b_init = 0, tmp_init = 0;
	size_t bit_count, i;

	if(!result || !base_val || !exp || !mod) return edk_FAILED;
	if(mod->sign == 0 || mod->used == 0) return edk_FAILED;  /* mod = 0 */

	/* exp < 0 はサポートしない */
	if(exp->sign < 0) return edk_FAILED;

	/* exp == 0 → result = 1 */
	if(exp->sign == 0 || exp->used == 0){
		return dkcBigIntFromInt(result, 1);
	}

	/* r = 1 */
	if(dkcBigIntCreate(&r, mod->used + 1) != edk_SUCCEEDED) return edk_FAILED;
	r_init = 1;
	if(dkcBigIntFromInt(&r, 1) != edk_SUCCEEDED) goto ModPowError;

	/* b = base_val mod mod */
	if(dkcBigIntCreate(&b, base_val->used) != edk_SUCCEEDED) goto ModPowError;
	b_init = 1;
	if(dkcBigIntMod(&b, base_val, mod) != edk_SUCCEEDED) goto ModPowError;
	if(b.sign < 0){
		/* 負の場合は正の剰余に */
		DKC_BIGINTEGER tmp2;
		if(dkcBigIntCreate(&tmp2, mod->used + 1) != edk_SUCCEEDED) goto ModPowError;
		if(dkcBigIntAdd(&tmp2, &b, mod) != edk_SUCCEEDED){
			dkcBigIntFree(&tmp2);
			goto ModPowError;
		}
		dkcBigIntCopy(&b, &tmp2);
		dkcBigIntFree(&tmp2);
	}

	if(dkcBigIntCreate(&tmp, (mod->used + 1) * 2) != edk_SUCCEEDED) goto ModPowError;
	tmp_init = 1;

	/* バイナリ法 (square-and-multiply) */
	bit_count = dkcBigIntBitCount(exp);

	for(i = bit_count; i > 0; i--){
		size_t bit_idx = i - 1;
		size_t limb_idx = bit_idx / BIGINT_LIMB_BITS;
		size_t bit_off = bit_idx % BIGINT_LIMB_BITS;
		int bit_val;

		/* r = r * r mod mod */
		if(dkcBigIntMul(&tmp, &r, &r) != edk_SUCCEEDED) goto ModPowError;
		if(dkcBigIntMod(&r, &tmp, mod) != edk_SUCCEEDED) goto ModPowError;
		if(r.sign < 0){
			if(dkcBigIntAdd(&tmp, &r, mod) != edk_SUCCEEDED) goto ModPowError;
			if(dkcBigIntCopy(&r, &tmp) != edk_SUCCEEDED) goto ModPowError;
		}

		bit_val = (limb_idx < exp->used) ?
			(int)((exp->digits[limb_idx] >> bit_off) & 1) : 0;

		if(bit_val){
			/* r = r * b mod mod */
			if(dkcBigIntMul(&tmp, &r, &b) != edk_SUCCEEDED) goto ModPowError;
			if(dkcBigIntMod(&r, &tmp, mod) != edk_SUCCEEDED) goto ModPowError;
			if(r.sign < 0){
				if(dkcBigIntAdd(&tmp, &r, mod) != edk_SUCCEEDED) goto ModPowError;
				if(dkcBigIntCopy(&r, &tmp) != edk_SUCCEEDED) goto ModPowError;
			}
		}
	}

	if(dkcBigIntCopy(result, &r) != edk_SUCCEEDED) goto ModPowError;

	dkcBigIntFree(&r);
	dkcBigIntFree(&b);
	dkcBigIntFree(&tmp);
	return edk_SUCCEEDED;

ModPowError:
	if(r_init) dkcBigIntFree(&r);
	if(b_init) dkcBigIntFree(&b);
	if(tmp_init) dkcBigIntFree(&tmp);
	return edk_FAILED;
}

DKC_EXTERN size_t WINAPI dkcBigIntBitCount(const DKC_BIGINTEGER *bi)
{
	size_t top_limb;
	uint32 val;
	size_t bits;

	if(!bi || bi->used == 0) return 0;

	top_limb = bi->used - 1;
	val = bi->digits[top_limb];
	bits = top_limb * BIGINT_LIMB_BITS;

	while(val > 0){
		bits++;
		val >>= 1;
	}

	return bits;
}
