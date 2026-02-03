/**
@author d金魚
@since 2005/01/25
@see http://seclan.dll.jp/stdbool.hを参考に実装
@note
絶対にtypedef _Bool bool; としてはいけない。
なぜなら、C++でCのヘッダを使う場合、コンパイラ側が混乱するからである。
typedef bool _Boolの場合は不明･･･○|￣|＿
しかし、trueとfalseはCとC++は同値な為問題無い。
*/

#ifndef DKUTIL_CSTD_STDBOOL_H
#define DKUTIL_CSTD_STDBOOL_H



#ifndef __cplusplus

#if !defined(_Bool) //|| !defined(_MSC_VER)
#	define _Bool unsigned char
#endif

#define true  1
#define false 0

#endif // __cplusplus 
#endif //end of include once
