# Microsoft Developer Studio Project File - Name="dkcCryptograph" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** 編集しないでください **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=dkcCryptograph - Win32 DebugMT
!MESSAGE これは有効なﾒｲｸﾌｧｲﾙではありません。 このﾌﾟﾛｼﾞｪｸﾄをﾋﾞﾙﾄﾞするためには NMAKE を使用してください。
!MESSAGE [ﾒｲｸﾌｧｲﾙのｴｸｽﾎﾟｰﾄ] ｺﾏﾝﾄﾞを使用して実行してください
!MESSAGE 
!MESSAGE NMAKE /f "dkcCryptograph.mak".
!MESSAGE 
!MESSAGE NMAKE の実行時に構成を指定できます
!MESSAGE ｺﾏﾝﾄﾞ ﾗｲﾝ上でﾏｸﾛの設定を定義します。例:
!MESSAGE 
!MESSAGE NMAKE /f "dkcCryptograph.mak" CFG="dkcCryptograph - Win32 DebugMT"
!MESSAGE 
!MESSAGE 選択可能なﾋﾞﾙﾄﾞ ﾓｰﾄﾞ:
!MESSAGE 
!MESSAGE "dkcCryptograph - Win32 Release" ("Win32 (x86) Static Library" 用)
!MESSAGE "dkcCryptograph - Win32 Debug" ("Win32 (x86) Static Library" 用)
!MESSAGE "dkcCryptograph - Win32 DebugMT" ("Win32 (x86) Static Library" 用)
!MESSAGE "dkcCryptograph - Win32 ReleaseMT" ("Win32 (x86) Static Library" 用)
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "dkcCryptograph - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD BASE RSC /l 0x411 /d "NDEBUG"
# ADD RSC /l 0x411 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "dkcCryptograph - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD BASE RSC /l 0x411 /d "_DEBUG"
# ADD RSC /l 0x411 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"Debug\dkcCryptographd.lib"

!ELSEIF  "$(CFG)" == "dkcCryptograph - Win32 DebugMT"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "dkcCryptograph___Win32_DebugMT"
# PROP BASE Intermediate_Dir "dkcCryptograph___Win32_DebugMT"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "dkcCryptograph___Win32_DebugMT"
# PROP Intermediate_Dir "dkcCryptograph___Win32_DebugMT"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD BASE RSC /l 0x411 /d "_DEBUG"
# ADD RSC /l 0x411 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"DebugMT\dkcCryptographMTd.lib"

!ELSEIF  "$(CFG)" == "dkcCryptograph - Win32 ReleaseMT"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "dkcCryptograph___Win32_ReleaseMT"
# PROP BASE Intermediate_Dir "dkcCryptograph___Win32_ReleaseMT"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "dkcCryptograph___Win32_ReleaseMT"
# PROP Intermediate_Dir "dkcCryptograph___Win32_ReleaseMT"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD BASE RSC /l 0x411 /d "NDEBUG"
# ADD RSC /l 0x411 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"ReleaseMT\dkcCryptographMT.lib"

!ENDIF 

# Begin Target

# Name "dkcCryptograph - Win32 Release"
# Name "dkcCryptograph - Win32 Debug"
# Name "dkcCryptograph - Win32 DebugMT"
# Name "dkcCryptograph - Win32 ReleaseMT"
# Begin Group "hash"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\dkcCRC.c
# End Source File
# Begin Source File

SOURCE=.\dkcCRC.h
# End Source File
# Begin Source File

SOURCE=.\dkcHash.c
# End Source File
# Begin Source File

SOURCE=.\dkcHash.h
# End Source File
# Begin Source File

SOURCE=.\dkcMD5.c
# End Source File
# Begin Source File

SOURCE=.\dkcMD5.h
# End Source File
# Begin Source File

SOURCE=.\dkcSHA.c
# End Source File
# Begin Source File

SOURCE=.\dkcSHA.h
# End Source File
# Begin Source File

SOURCE=.\dkcSHA1.c
# End Source File
# Begin Source File

SOURCE=.\dkcSHA1.h
# End Source File
# Begin Source File

SOURCE=.\dkcSHA256.c
# End Source File
# Begin Source File

SOURCE=.\dkcSHA256.h
# End Source File
# Begin Source File

SOURCE=.\dkcSHA384.c
# End Source File
# Begin Source File

SOURCE=.\dkcSHA384.h
# End Source File
# Begin Source File

SOURCE=.\dkcSHA512.c
# End Source File
# Begin Source File

SOURCE=.\dkcSHA512.h
# End Source File
# Begin Source File

SOURCE=.\md5.c
# End Source File
# Begin Source File

SOURCE=.\md5.h
# End Source File
# Begin Source File

SOURCE=.\st.c
# End Source File
# Begin Source File

SOURCE=.\st.h
# End Source File
# End Group
# Begin Group "cryptograph"

# PROP Default_Filter ""
# Begin Group "rijndael"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\dkcRijndael.c
# End Source File
# Begin Source File

SOURCE=.\dkcRijndael.h
# End Source File
# Begin Source File

SOURCE=".\rijndael-alg-fst.c"
# End Source File
# Begin Source File

SOURCE=".\rijndael-alg-fst.h"
# End Source File
# Begin Source File

SOURCE=".\rijndael-api-fst.c"
# End Source File
# Begin Source File

SOURCE=".\rijndael-api-fst.h"
# End Source File
# End Group
# Begin Source File

SOURCE=.\dkcArcfour.c
# End Source File
# Begin Source File

SOURCE=.\dkcArcfour.h
# End Source File
# Begin Source File

SOURCE=.\dkcBlowfish.c
# End Source File
# Begin Source File

SOURCE=.\dkcBlowfish.h
# End Source File
# Begin Source File

SOURCE=.\dkcCryptograph.c
# End Source File
# Begin Source File

SOURCE=.\dkcCryptograph.h
# End Source File
# Begin Source File

SOURCE=.\dkcHC256.c
# End Source File
# Begin Source File

SOURCE=.\dkcHC256.h
# End Source File
# Begin Source File

SOURCE=.\dkcHMAC.c
# End Source File
# Begin Source File

SOURCE=.\dkcHMAC.h
# End Source File
# Begin Source File

SOURCE=.\dkcSemiRealRandom.c
# End Source File
# Begin Source File

SOURCE=.\dkcSemiRealRandom.h
# End Source File
# Begin Source File

SOURCE=.\dkcSNOW20.c
# End Source File
# Begin Source File

SOURCE=.\dkcSNOW20.h
# End Source File
# Begin Source File

SOURCE=.\dkcVernam.c
# End Source File
# Begin Source File

SOURCE=.\dkcVernam.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\dkcLibCryptograph.h
# End Source File
# End Target
# End Project
