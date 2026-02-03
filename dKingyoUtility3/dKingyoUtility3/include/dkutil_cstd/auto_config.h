/**
@author d金魚
@file auto_config.h
@brief 自動的に設定するコンパイルの設定
@since 2005/01/17
*/


#ifndef DKUTIL_CSTD_AUTO_CONFIG_H
#define DKUTIL_CSTD_AUTO_CONFIG_H

#ifndef DKUTIL_CSTD_NOT_USE_AUTO_CONFIG

#	if _MSC_VER >= 1100
		///inline assemblerを使う
#		define DKUTIL_CSTD_MSVC_IASM 1
#	else
#		define DKUTIL_CSTD_MSVC_IASM 0
#	endif



#endif //end of DKUTIL_CSTD_NOT_USE_AUTO_CONFIG

#ifndef DKUTIL_CSTD_PREFIX 
#	define DKUTIL_CSTD_PREFIX dkcstd_
#endif

#endif //end of include once

