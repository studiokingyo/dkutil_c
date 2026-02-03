/**
@author d金魚
@file macro.h
@brief C言語用マクロ
@since 2003/9/13 reconstruct:2005/01/17
*/


#ifndef DKUTIL_CSTD_MACRO_H
#define DKUTIL_CSTD_MACRO_H


///正統派 整数、実数専用スワップ @warning (&a)!=(&b)であること。すなわち同じ変数を引数として入れない事。
#define SWAP_NUM(a,b) \
    (a) = (b) - (a) ;\
    (b) -= (a) ;\
    (a) += (b) 
/*!
@see SWAP_NUM()
@param ex[in] 型名　unsigned char 等
*/
#define SWAP_NUM_EX(a,b,ex) \
	(ex)(a) = (ex)((b) - (a) ); \
	(ex)(b) = (ex)((b) - (a) ); \
	(ex)(a) = (ex)((a) + (b) )

///@param t[in] register int とか @note (&a)==(&b)でもOKです。
#define SWAP_TEMP(t,a,b){\
	t _temp____ = a;\
	a = b;\
	b = _temp____;\
}



#ifdef WIN32
///32bit整数専用swap (&a)==(&b)でもOK
#define SWAP_FAST32(a,b)\
	_asm mov eax,a\
	_asm mov ebx,b\
	_asm mov a,ebx\
	_asm mov b,eax
///パーシャルレジスタのストールがおきるかもしれない。
#define SWAP_FAST16(a,b)\
	_asm mov ax,a\
	_asm mov bx,b\
	_asm mov a,bx\
	_asm mov b,ax
///@see SWAP_FAST16()
#define SWAP_FAST8(a,b)\
	_asm mov AH,a\
	_asm mov BH,b\
	_asm mov a,BH\
	_asm mov b,AH

#else
///@see SWAP_TEMP()
#define SWAP_FAST32(a,b) SWAP_TEMP(register uint32,a,b)
#define SWAP_FAST16(a,b)	SWAP_TEMP(register uint16,a,b)
#define SWAP_FAST8(a,b)	SWAP_TEMP(register uint8,a,b)

#endif
/*!
@note

@code
//example
UINT get_offset = SM_OFFSET(_WIN32_FIND_DATA *,dwReserved1);
@endcode
*/
///構造体のオフセットを求める Struct Member Offset
#define SM_OFFSET(p_type,field)     ((unsigned int)&(((p_type)NULL)->field))

///RECT構造体に入れる。
#define SET_RECT(rect, l, t, r, b) \
{\
    rect##.left   = l;\
    rect##.top    = t;\
    rect##.right  = r;\
    rect##.bottom = b;\
}


///配列の数を数える
#define DKUTIL_ARRAY_NUMOF(array) (sizeof(array)/sizeof(array[0]))
///名前を合わせる。
#define DKUTIL_NAMELINK(X,Y) X##Y
///シグネチャの名前を作る。
#define DKUTIL_MAKESIGNATURE(X) DKUTIL_NAMELINK(X,__LINE__)
///@param id[in] example 'abcd'  @return unsigned int
#define DKUTIL_MAKE_UINT_ID(id) ((((UINT)(id) & 0xFF) << 24) |     \
                  (((UINT)(id) & 0xFF00) << 8) |    \
                  (((UINT)(id) & 0xFF0000) >> 8) |  \
                  (((UINT)(id) & 0xFF000000) >> 24))

///http://d.hatena.ne.jp/paserry/ paserry氏に教えていただく。m(_ _)m 
#define DKUTIL_FLAG_DOWN(x,n) {x &= ~n;}//{(x & n) ? x |= (x^n) : 0;}//{x |= (x^n);}
///フラグアップマクロ。これは常識。
#define DKUTIL_FLAG_UP(x,n) {x |= n;}

///UINT系変数の左からNビット目のフラグをアップさせる。注意：0からはじまると思う。

#define DKUTIL_FLAG_UP_N(x,n) DKUTIL_FLAG_UP(x,(1 << n))
///@see DKUTIL_FLAG_UP_N() 左からnビット目のフラグをダウンさせる。

#define DKUTIL_FLAG_DOWN_N(x,n) DKUTIL_FLAG_DOWN(x,( 1<<n ))
//#define DKUTIL_FLAG_DOWN_N(x,n) {x &= (x^ ( 1<<n ) );}
//#define DKUTIL_FLAG_UP_N(x,n) {x |= (1 << n);}


#ifdef __cplusplus

/*!
* 後ろの方に沢山引数をつけられるようにするva_listの初期化。<br>
* buff = charのバッファ <br>
* buffnum = buffのサイズ(バイト単位)<br>
* origin = 最初の文字列 <br>
* 例：例えば、普通に書くとこんな感じ。<br>
* <PRE>
* void ERROR_BOX(char *str,...){
* 	char s[512];
* 	va_list VaList ;
* 	va_start( VaList , str ) ;
* 	vsprintf( s , str , VaList ) ;
* 	va_end( VaList ) ;
* 	MessageBox(NULL,s,"ERROR_BOX",MB_OK);
* }
* このマクロはこのように使う。
* void ERROR_BOX(char *str,...){<
* 	char s[512];
* 	SET_VA_LIST(s,512,str);
* 	MessageBox(NULL,s,"ERROR_BOX",MB_OK);
* }
* </PRE>
* どうだ！！！これでかなり面倒な文が減った！！<br>
*	@param buff [out] 文字列バッファ
*	@param buffnum [in] バッファのサイズ(バイト単位
*	@param origin [in] 元の文字列
@note
(2004/1/2 ちょっと直す･･･なんか１BYTE無駄にしているような
*/
///沢山引数をつける関数のサポート＾＾；
#define SET_VA_LIST(buff,buffnum,origin) \
{\
	va_list VaList;\
	va_start( VaList , origin ) ;\
	int r;\
	r = _vsnprintf( buff ,buffnum - 1, origin , VaList );\
	if(0 > r){\
		buff[buffnum - 1] = '\0';\
	}else{\
		buff[r+1] = '\0';\
	}\
	va_end( VaList ); \
}


#	define DKUTIL_SAFE_REALLOC(b,s) ::realloc(b,s)
#	define DKUTIL_SAFE_MALLOC(s) ::malloc(s)
#	define DKUTIL_SAFE_FREE(a) if(a){::free(a);a=NULL;}
	
///構造体を初期化する。(構造体へのポインタではない。）
#	define DKUTIL_STRUCTURE_INIT(s) ::memset(&(s),0,sizeof(s))
///ZeroMemory()みたいなもの
#	define DKUTIL_MEMZERO(mem,size) ::memset(mem,0,size)

#else // for c lang



#define DKUTIL_SAFE_REALLOC(b,s) realloc(b,s)
#define DKUTIL_SAFE_MALLOC(s) malloc(s)
#define DKUTIL_SAFE_FREE(a) if(a){free(a);a=NULL;}
#define DKUTIL_STRUCTURE_INIT(s) memset(&(s),0,sizeof(s))
///ZeroMemory()みたいなもの
#	define DKUTIL_MEMZERO(mem__,size__) memset(mem__,0,size__)
#endif

///配列を初期化する
#define DKUTIL_ARRAY_ZERO(arr) DKUTIL_MEMZERO(arr,sizeof(arr))
///CHARの配列を初期化する。
#define NULL_CHAR_ARRAY(s) DKUTIL_ARRAY_ZERO(s)

#define SET_VA_LIST_C(VaList,r,buff,buffnum,origin) \
{\
	va_start( VaList , origin ) ;\
	r = _vsnprintf( buff ,buffnum - 1, origin , VaList );\
	if(0 > r){\
		buff[buffnum - 1] = '\0';\
	}else{\
		buff[r+1] = '\0';\
	}\
	va_end( VaList ); \
}

#define DKUTIL_BIT_TO_BYTE(a) (a / 8)

/// this version of the macro is fastest on Pentium 3 and Pentium 4 with MSVC 6 SP5 w/ Processor Pack @note by cryptopp
#define dkcmGETBYTE(x, y)		((x)>>(8*(y)))
#define dkcmGETBYTE8(x,y)				(uint8)dkcmGETBYTE(x,y)
#define dkcmGETBYTE16(x,y)				(uint16)dkcmGETBYTE(x,y)
#define dkcmGETBYTE32(x,y)	(uint32)dkcmGETBYTE8(x,y)

// @note by cryptopp
#if defined(__GNUC__) || defined(__MWERKS__)
# define dkcmUINT64DEFINE(x) x##LL
#elif defined(_MSC_VER) || defined(__BCPLUSPLUS__)
#	define dkcmUINT64DEFINE(x) x##ui64
#else
#	define dkcmUINT64DEFINE(x) (x)
#endif

#endif //end of include once

