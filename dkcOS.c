
#define DKUTIL_C_OS_C
#include "dkcOS.h"

#ifdef WIN32

int WINAPI dkcGetOSVersion(){
	int r = -1;
	OSVERSIONINFO os;
	os.dwOSVersionInfoSize=sizeof(os);

	 
	if(GetVersionEx(&os)==0)
		goto End;

	if(os.dwMajorVersion<=3){
		//というかWindows3.1使う馬鹿いないし…また、
		//ここらへん間違っていそう…参考にはしないように
		if(os.dwMinorVersion==51){
			r = enuWindowsNT3_51;
			goto End;
		}
		if(os.dwPlatformId==VER_PLATFORM_WIN32s){
			r = enuWindows3_1;
			goto End;
		}
	}

	switch(os.dwPlatformId){
		case VER_PLATFORM_WIN32s:
			r =  enuWindows3_1;
			break;
			
		case VER_PLATFORM_WIN32_WINDOWS:
			if(os.dwMajorVersion >=4)
			{
				if(os.dwMinorVersion<10){
					r = enuWindows95;
					goto End;
				}
				if(os.dwMinorVersion<90){
					r = enuWindows98;
					goto End;
				}

				if(os.dwMinorVersion>=90){
					r = enuWindowsMe;
					goto End;
				}else{
					r = -2;
					goto End;
				}
			}
			if(os.dwMajorVersion==5){//念のため
				
				if(os.dwMinorVersion==0){
					r = enuWindows2000;
					goto End;
				}
				if(os.dwMinorVersion>0){
					r = enuWindowsXP;
					goto End;
				}else{
					r = -2;
					goto End;
				}
			}
			break;
				
		
		case VER_PLATFORM_WIN32_NT:
			if(os.dwMajorVersion==4)
				r = enuWindowsNT4_0;
			
			if(os.dwMajorVersion==5){
				if(os.dwMinorVersion==0)
					r =  enuWindows2000;
				if(os.dwMinorVersion>0){
					r = enuWindowsXP;
				}
				else{
					return -2;
				}
			}
			return -2;
			break;
		default:
			return -2;
			break;
	}
End:
	return r;
}

DKC_INLINE BOOL dkcIsOS9x(){
	int r = dkcGetOSVersion();
	return (r==enuWindows98 || r == enuWindows95 || r==enuWindowsMe);
}

DKC_INLINE BOOL dkcIsOSNT(){
	return !dkcIsOS9x();
}

//#undef 	OSVERSIONINFOEX
//#define OSVERSIONINFOEX DKC_OSVERSIONINFOEX 	

#if (_MSC_VER > 1200)

int WINAPI dkcGetOSVersionEx(int *Extension)
{
	int result=-1;
	int ext=0;
	OSVERSIONINFOEX osvi={0};
	BOOL SucceededFlag=FALSE;

	
	osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);

	SucceededFlag = GetVersionEx((OSVERSIONINFO *)&osvi);
	if (!SucceededFlag) {
		osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
		if(!GetVersionEx((OSVERSIONINFO *)&osvi)){
			return -1;
		}
	}

	switch (osvi.dwPlatformId)
	{
	case VER_PLATFORM_WIN32_NT:
		if (osvi.dwMajorVersion <= 4) {
			result = enuWindowsNT4_0;
		}

		if (osvi.dwMajorVersion == 5 && osvi.dwMinorVersion == 0) {
			result = enuWindows2000;
		}
		if (osvi.dwMajorVersion == 5 && osvi.dwMinorVersion == 1){
			result = enuWindowsXP;
		}
		if (SucceededFlag) {		// GetVersionEx 関数からの情報を使う
			if (osvi.wProductType == VER_NT_WORKSTATION) {

				if (osvi.wSuiteMask & VER_SUITE_PERSONAL) {
					ext = edkOSE_HomeEdition;
				}else {
					ext = edkOSE_Professional;
				}
			}else if (osvi.wProductType == VER_NT_SERVER) {
				if (osvi.dwMajorVersion == 5 && osvi.dwMinorVersion == 1) {
					result = enuWindows_NET;
				}

				if (osvi.wSuiteMask & VER_SUITE_DATACENTER) {
					ext = edkOSE_DataCenter_Server;
				}else if (osvi.wSuiteMask & VER_SUITE_ENTERPRISE) {
					if( osvi.dwMajorVersion == 4 ) {
						ext = edkOSE_Advanced_Server;
					}
					else {
						ext = edkOSE_Enterprise_Server;
					}
				}
				else if (osvi.wSuiteMask == VER_SUITE_BLADE) {
					ext = edkOSE_Web_Server;
				}
				else {
					ext = edkOSE_Server;
				}
			}
		}
		else {		// 初期の Windows NT では、レジストリを使う
			HKEY hKey;
			char szProductType[80];
			DWORD dwBufLen;

			RegOpenKeyEx(HKEY_LOCAL_MACHINE, 
						 "SYSTEM\\CurrentControlSet\\Control\\ProductOptions", 
						 0, KEY_QUERY_VALUE, &hKey );
			RegQueryValueEx(hKey, "ProductType", NULL, NULL, 
							(LPBYTE)szProductType, &dwBufLen);
			RegCloseKey(hKey);

			if (lstrcmpi("WINNT", szProductType) == 0) {
				ext= edkOSE_Professional;
			}
			if (lstrcmpi("LANMANNT", szProductType) == 0) {
				ext = edkOSE_Server;
			}
			if (lstrcmpi( "SERVERNT", szProductType) == 0) {
				ext = edkOSE_Advanced_Server;
			}
		}

		break;

	case VER_PLATFORM_WIN32_WINDOWS:
		if (osvi.dwMajorVersion == 4 && osvi.dwMinorVersion == 0) {
			result = enuWindows95;
			if (osvi.szCSDVersion[1] == 'C' || osvi.szCSDVersion[1] == 'B') {
				ext = edkOSE_OSR2;
			}
		}

		if (osvi.dwMajorVersion == 4 && osvi.dwMinorVersion == 10) {
			result = enuWindows98;
			if (osvi.szCSDVersion[1] == 'A') {
				ext = enuOSE_SE;
			}
		}

		if (osvi.dwMajorVersion == 4 && osvi.dwMinorVersion == 90) {
		 	result = enuWindowsME;
		}

		break;
	default:
		return -1;
	}//end of switch

	if(Extension){
		*Extension = ext;
	}

	return result;
}

//#undef 	OSVERSIONINFOEX
#endif


#endif
