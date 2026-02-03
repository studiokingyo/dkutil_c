# Microsoft Developer Studio Project File - Name="dKingyoUtility3" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** 編集しないでください **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=dKingyoUtility3 - Win32 DebugMT
!MESSAGE これは有効なﾒｲｸﾌｧｲﾙではありません。 このﾌﾟﾛｼﾞｪｸﾄをﾋﾞﾙﾄﾞするためには NMAKE を使用してください。
!MESSAGE [ﾒｲｸﾌｧｲﾙのｴｸｽﾎﾟｰﾄ] ｺﾏﾝﾄﾞを使用して実行してください
!MESSAGE 
!MESSAGE NMAKE /f "dKingyoUtility3.mak".
!MESSAGE 
!MESSAGE NMAKE の実行時に構成を指定できます
!MESSAGE ｺﾏﾝﾄﾞ ﾗｲﾝ上でﾏｸﾛの設定を定義します。例:
!MESSAGE 
!MESSAGE NMAKE /f "dKingyoUtility3.mak" CFG="dKingyoUtility3 - Win32 DebugMT"
!MESSAGE 
!MESSAGE 選択可能なﾋﾞﾙﾄﾞ ﾓｰﾄﾞ:
!MESSAGE 
!MESSAGE "dKingyoUtility3 - Win32 Release" ("Win32 (x86) Dynamic-Link Library" 用)
!MESSAGE "dKingyoUtility3 - Win32 Debug" ("Win32 (x86) Dynamic-Link Library" 用)
!MESSAGE "dKingyoUtility3 - Win32 DebugMT" ("Win32 (x86) Dynamic-Link Library" 用)
!MESSAGE "dKingyoUtility3 - Win32 ReleaseMT" ("Win32 (x86) Dynamic-Link Library" 用)
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "dKingyoUtility3 - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "DKINGYOUTILITY3_EXPORTS" /YX /FD /c
# ADD CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "DKINGYOUTILITY3_EXPORTS" /YX /FD /D /NODEFAULTLIB:oldnames /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x411 /d "NDEBUG"
# ADD RSC /l 0x411 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /map /machine:I386
# SUBTRACT LINK32 /nodefaultlib

!ELSEIF  "$(CFG)" == "dKingyoUtility3 - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "DKINGYOUTILITY3_EXPORTS" /YX /FD /GZ /c
# ADD CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "DKINGYOUTILITY3_EXPORTS" /FR /YX /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x411 /d "_DEBUG"
# ADD RSC /l 0x411 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /debug /machine:I386 /out:"Debug/dKingyoUtility3d.dll" /pdbtype:sept

!ELSEIF  "$(CFG)" == "dKingyoUtility3 - Win32 DebugMT"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "dKingyoUtility3___Win32_DebugMT"
# PROP BASE Intermediate_Dir "dKingyoUtility3___Win32_DebugMT"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "DebugMT"
# PROP Intermediate_Dir "DebugMT"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "DKINGYOUTILITY3_EXPORTS" /YX /FD /GZ /c
# ADD CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "DKINGYOUTILITY3_EXPORTS" /FR /YX /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x411 /d "_DEBUG"
# ADD RSC /l 0x411 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /debug /machine:I386 /out:"DebugMT/dKingyoUtility3MTd.dll" /pdbtype:sept

!ELSEIF  "$(CFG)" == "dKingyoUtility3 - Win32 ReleaseMT"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "dKingyoUtility3___Win32_ReleaseMT"
# PROP BASE Intermediate_Dir "dKingyoUtility3___Win32_ReleaseMT"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "ReleaseMT"
# PROP Intermediate_Dir "ReleaseMT"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "DKINGYOUTILITY3_EXPORTS" /YX /FD /c
# ADD CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "DKINGYOUTILITY3_EXPORTS" /FR /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x411 /d "NDEBUG"
# ADD RSC /l 0x411 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /map /machine:I386 /out:"ReleaseMT/dKingyoUtility3MT.dll"

!ENDIF 

# Begin Target

# Name "dKingyoUtility3 - Win32 Release"
# Name "dKingyoUtility3 - Win32 Debug"
# Name "dKingyoUtility3 - Win32 DebugMT"
# Name "dKingyoUtility3 - Win32 ReleaseMT"
# Begin Group "common"

# PROP Default_Filter ""
# Begin Group "filesystem"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\dkcFileSystems.h
# End Source File
# Begin Source File

SOURCE=..\dkcGenericFileSystem.c
# End Source File
# Begin Source File

SOURCE=..\dkcGenericFileSystem.h
# End Source File
# Begin Source File

SOURCE=..\dkcSafeFileSystem.c
# End Source File
# Begin Source File

SOURCE=..\dkcSafeFileSystem.h
# End Source File
# Begin Source File

SOURCE=..\dkcSJISFileSystem.c
# End Source File
# Begin Source File

SOURCE=..\dkcSJISFileSystem.h
# End Source File
# Begin Source File

SOURCE=..\dkcStream.c
# End Source File
# Begin Source File

SOURCE=..\dkcStream.h
# End Source File
# End Group
# Begin Source File

SOURCE=..\dkcBit.c
# End Source File
# Begin Source File

SOURCE=..\dkcBit.h
# End Source File
# Begin Source File

SOURCE=..\dkcDefined.h
# End Source File
# Begin Source File

SOURCE=..\dkcDLL.c
# End Source File
# Begin Source File

SOURCE=..\dkcDLL.h
# End Source File
# Begin Source File

SOURCE=..\dkcLua.c
# End Source File
# Begin Source File

SOURCE=..\dkcLua.h
# End Source File
# Begin Source File

SOURCE=..\dkcMath.c
# End Source File
# Begin Source File

SOURCE=..\dkcMath.h
# End Source File
# Begin Source File

SOURCE=..\dkcMemoryPool.c
# End Source File
# Begin Source File

SOURCE=..\dkcMemoryPool.h
# End Source File
# Begin Source File

SOURCE=..\dkcOS.c
# End Source File
# Begin Source File

SOURCE=..\dkcOS.h
# End Source File
# Begin Source File

SOURCE=..\dkcOSIndependent.c
# End Source File
# Begin Source File

SOURCE=..\dkcOSIndependent.h
# End Source File
# Begin Source File

SOURCE=..\dkcRegex.c
# End Source File
# Begin Source File

SOURCE=..\dkcRegex.h
# End Source File
# Begin Source File

SOURCE=..\dkcSort.c
# End Source File
# Begin Source File

SOURCE=..\dkcSort.h
# End Source File
# Begin Source File

SOURCE=..\dkcStdio.c
# End Source File
# Begin Source File

SOURCE=..\dkcStdio.h
# End Source File
# Begin Source File

SOURCE=..\dkcTaskEngine.c
# End Source File
# Begin Source File

SOURCE=..\dkcTaskEngine.h
# End Source File
# Begin Source File

SOURCE=..\dkcThread.c
# End Source File
# Begin Source File

SOURCE=..\dkcThread.h
# End Source File
# Begin Source File

SOURCE=..\dkcThreadLock.c
# End Source File
# Begin Source File

SOURCE=..\dkcThreadLock.h
# End Source File
# Begin Source File

SOURCE=..\dkcUniqueID.c
# End Source File
# Begin Source File

SOURCE=..\dkcUniqueID.h
# End Source File
# End Group
# Begin Group "hash"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\dkcCRC.c
# End Source File
# Begin Source File

SOURCE=..\dkcCRC.h
# End Source File
# Begin Source File

SOURCE=..\dkcHash.c
# End Source File
# Begin Source File

SOURCE=..\dkcHash.h
# End Source File
# Begin Source File

SOURCE=..\dkcMD2.c
# End Source File
# Begin Source File

SOURCE=..\dkcMD2.h
# End Source File
# Begin Source File

SOURCE=..\dkcMD4.c
# End Source File
# Begin Source File

SOURCE=..\dkcMD4.h
# End Source File
# Begin Source File

SOURCE=..\dkcMD5.c
# End Source File
# Begin Source File

SOURCE=..\dkcMD5.h
# End Source File
# Begin Source File

SOURCE=..\dkcMessageDigest.h
# End Source File
# Begin Source File

SOURCE=..\dkcSHA.c
# End Source File
# Begin Source File

SOURCE=..\dkcSHA.h
# End Source File
# Begin Source File

SOURCE=..\dkcSHA1.c
# End Source File
# Begin Source File

SOURCE=..\dkcSHA1.h
# End Source File
# Begin Source File

SOURCE=..\dkcSHA256.c
# End Source File
# Begin Source File

SOURCE=..\dkcSHA256.h
# End Source File
# Begin Source File

SOURCE=..\dkcSHA384.c
# End Source File
# Begin Source File

SOURCE=..\dkcSHA384.h
# End Source File
# Begin Source File

SOURCE=..\dkcSHA512.c
# End Source File
# Begin Source File

SOURCE=..\dkcSHA512.h
# End Source File
# Begin Source File

SOURCE=..\md5_vc_mmx.h
# End Source File
# Begin Source File

SOURCE=..\md_misc.h
# End Source File
# Begin Source File

SOURCE=..\st.c
# End Source File
# Begin Source File

SOURCE=..\st.h
# End Source File
# End Group
# Begin Group "compression"

# PROP Default_Filter ""
# Begin Group "win32"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\dkcUnityArchivers.c
# End Source File
# Begin Source File

SOURCE=..\dkcUnityArchivers.h
# End Source File
# End Group
# Begin Source File

SOURCE=..\dkcBlockSort.c
# End Source File
# Begin Source File

SOURCE=..\dkcBlockSort.h
# End Source File
# Begin Source File

SOURCE=..\dkcLZSS.c
# End Source File
# Begin Source File

SOURCE=..\dkcLZSS.h
# End Source File
# Begin Source File

SOURCE=..\dkcLZW.c
# End Source File
# Begin Source File

SOURCE=..\dkcLZW.h
# End Source File
# Begin Source File

SOURCE=..\dkcRLE.c
# End Source File
# Begin Source File

SOURCE=..\dkcRLE.h
# End Source File
# End Group
# Begin Group "cryptograph"

# PROP Default_Filter ""
# Begin Group "rijndael"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\dkcRijndael.c
# End Source File
# Begin Source File

SOURCE=..\dkcRijndael.h
# End Source File
# Begin Source File

SOURCE="..\rijndael-alg-fst.c"
# End Source File
# Begin Source File

SOURCE="..\rijndael-alg-fst.h"
# End Source File
# Begin Source File

SOURCE="..\rijndael-api-fst.c"
# End Source File
# Begin Source File

SOURCE="..\rijndael-api-fst.h"
# End Source File
# End Group
# Begin Source File

SOURCE=..\dkcArcfour.c
# End Source File
# Begin Source File

SOURCE=..\dkcArcfour.h
# End Source File
# Begin Source File

SOURCE=..\dkcBlowfish.c
# End Source File
# Begin Source File

SOURCE=..\dkcBlowfish.h
# End Source File
# Begin Source File

SOURCE=..\dkcCryptograph.c
# End Source File
# Begin Source File

SOURCE=..\dkcCryptograph.h
# End Source File
# Begin Source File

SOURCE=..\dkcHC256.c
# End Source File
# Begin Source File

SOURCE=..\dkcHC256.h
# End Source File
# Begin Source File

SOURCE=..\dkcHMAC.c
# End Source File
# Begin Source File

SOURCE=..\dkcHMAC.h
# End Source File
# Begin Source File

SOURCE=..\dkcSemiRealRandom.c
# End Source File
# Begin Source File

SOURCE=..\dkcSemiRealRandom.h
# End Source File
# Begin Source File

SOURCE=..\dkcSNOW20.c
# End Source File
# Begin Source File

SOURCE=..\dkcSNOW20.h
# End Source File
# Begin Source File

SOURCE=..\dkcVernam.c
# End Source File
# Begin Source File

SOURCE=..\dkcVernam.h
# End Source File
# End Group
# Begin Group "container"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\dkc2Tree.c
# End Source File
# Begin Source File

SOURCE=..\dkc2Tree.h
# End Source File
# Begin Source File

SOURCE=..\dkcArrayOneByOne.c
# End Source File
# Begin Source File

SOURCE=..\dkcArrayOneByOne.h
# End Source File
# Begin Source File

SOURCE=..\dkcBuffer.c
# End Source File
# Begin Source File

SOURCE=..\dkcBuffer.h
# End Source File
# Begin Source File

SOURCE=..\dkcCircularMemoryStream.c
# End Source File
# Begin Source File

SOURCE=..\dkcCircularMemoryStream.h
# End Source File
# Begin Source File

SOURCE=..\dkcCircularStream.c
# End Source File
# Begin Source File

SOURCE=..\dkcCircularStream.h
# End Source File
# Begin Source File

SOURCE=..\dkcDeque.c
# End Source File
# Begin Source File

SOURCE=..\dkcDeque.h
# End Source File
# Begin Source File

SOURCE=..\dkcMemoryStream.c
# End Source File
# Begin Source File

SOURCE=..\dkcMemoryStream.h
# End Source File
# Begin Source File

SOURCE=..\dkcQueue.c
# End Source File
# Begin Source File

SOURCE=..\dkcQueue.h
# End Source File
# Begin Source File

SOURCE=..\dkcRedBlackTree.c
# End Source File
# Begin Source File

SOURCE=..\dkcRedBlackTree.h
# End Source File
# Begin Source File

SOURCE=..\dkcSingleList.c
# End Source File
# Begin Source File

SOURCE=..\dkcSingleList.h
# End Source File
# Begin Source File

SOURCE=..\dkcStack.c
# End Source File
# Begin Source File

SOURCE=..\dkcStack.h
# End Source File
# Begin Source File

SOURCE=..\dkcString.c
# End Source File
# Begin Source File

SOURCE=..\dkcString.h
# End Source File
# End Group
# Begin Group "routine"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\dcf_misc.h
# End Source File
# Begin Source File

SOURCE=..\dkcDCF.c
# End Source File
# Begin Source File

SOURCE=..\dkcDCF.h
# End Source File
# Begin Source File

SOURCE=..\dkcDCF01.c
# End Source File
# Begin Source File

SOURCE=..\dkcDCF01.h
# End Source File
# Begin Source File

SOURCE=..\dkcTelecomConfirmFile.c
# End Source File
# Begin Source File

SOURCE=..\dkcTelecomConfirmFile.h
# End Source File
# Begin Source File

SOURCE=..\dkcTelecomConfirmPacket.c
# End Source File
# Begin Source File

SOURCE=..\dkcTelecomConfirmPacket.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\bsdlicence.h
# End Source File
# Begin Source File

SOURCE=.\dKingyoUtility3.c
# End Source File
# Begin Source File

SOURCE=.\dKingyoUtility3.def

!IF  "$(CFG)" == "dKingyoUtility3 - Win32 Release"

!ELSEIF  "$(CFG)" == "dKingyoUtility3 - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "dKingyoUtility3 - Win32 DebugMT"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "dKingyoUtility3 - Win32 ReleaseMT"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\dKingyoUtility3.h
# End Source File
# Begin Source File

SOURCE=.\dKingyoUtility3d.def

!IF  "$(CFG)" == "dKingyoUtility3 - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "dKingyoUtility3 - Win32 Debug"

!ELSEIF  "$(CFG)" == "dKingyoUtility3 - Win32 DebugMT"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "dKingyoUtility3 - Win32 ReleaseMT"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\dKingyoUtility3MT.def

!IF  "$(CFG)" == "dKingyoUtility3 - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "dKingyoUtility3 - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "dKingyoUtility3 - Win32 DebugMT"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "dKingyoUtility3 - Win32 ReleaseMT"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\dKingyoUtility3MTd.def

!IF  "$(CFG)" == "dKingyoUtility3 - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "dKingyoUtility3 - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "dKingyoUtility3 - Win32 DebugMT"

!ELSEIF  "$(CFG)" == "dKingyoUtility3 - Win32 ReleaseMT"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\dll_resource.rc
# End Source File
# Begin Source File

SOURCE=.\resource.h
# End Source File
# End Target
# End Project
