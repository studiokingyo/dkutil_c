/*!
@note

http://techtips.belution.com/ja/vc/0074/
より、引用

    Products        Version                      OSVERSIONINFO
                            dwPlatformId dwMajorVersion dwMinorVersion dwBuildNumber
-------------------------------------------------------------------------------------
Windows 95          4.00.950      1            4            0            950
(Windows 95 + SP1)
Windows 95 OSR 2    4.00.950B      1            4            0            1111
(Windows 95 OSR 2.1)
Windows 95 OSR 2.5  4.00.950C      1            4            0            1212
Windows 98          4.10.1998      1            4          10            1998
Windows 98 SE      4.10.2222      1            4          10            2222
Windows Me          4.90.3000      1            4          90            3000
Windows NT 4.0      4.00.1381      2            4            0            1381
Windows 2000        5.00.2195      2            5            0            2195
-------------------------------------------------------------------------------------
*/
#ifndef DKUTIL_C_OS_H
#define DKUTIL_C_OS_H


#include "dkcOSIndependent.h"

#	ifdef WIN32


/*

typedef struct dkc_OSVERSIONINFOEX {
	DWORD dwOSVersionInfoSize;
	DWORD dwMajorVersion;
	DWORD dwMinorVersion;
	DWORD dwBuildNumber;
	DWORD dwPlatformId;
	TCHAR szCSDVersion[ 128 ];
	WORD wServicePackMajor;
	WORD wServicePackMinor;
	WORD wSuiteMask;
	BYTE wProductType;
	BYTE wReserved;
}DKC_OSVERSIONINFOEX; 
*/





/*!
@return 成功： edkOS / 失敗：-1 / 未知のOS:-2
*/
///OSのバージョンをゲットする。戻り値はedkOSを返します。
DKC_EXTERN int WINAPI dkcGetOSVersion(void);

DKC_EXTERN BOOL dkcIsOS9x();

DKC_EXTERN BOOL dkcIsOSNT();

#	else //posix

#	endif

#if (_MSC_VER > 1200)
/*!
@param Extension[in] edkOSExtensionの値が返る 0だと何でもない。
@return 成功：edkOS / 失敗：-1 / 未知のOS:-2
*/
DKC_EXTERN int WINAPI dkcGetOSVersionEx(int *Extension);

#endif

#ifndef DKUTIL_C_OS_C
#	include "dkcOS.h"
#endif


#endif //end of include once