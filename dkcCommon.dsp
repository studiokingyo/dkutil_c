# Microsoft Developer Studio Project File - Name="dkcCommon" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** 編集しないでください **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=dkcCommon - Win32 DebugMT
!MESSAGE これは有効なﾒｲｸﾌｧｲﾙではありません。 このﾌﾟﾛｼﾞｪｸﾄをﾋﾞﾙﾄﾞするためには NMAKE を使用してください。
!MESSAGE [ﾒｲｸﾌｧｲﾙのｴｸｽﾎﾟｰﾄ] ｺﾏﾝﾄﾞを使用して実行してください
!MESSAGE 
!MESSAGE NMAKE /f "dkcCommon.mak".
!MESSAGE 
!MESSAGE NMAKE の実行時に構成を指定できます
!MESSAGE ｺﾏﾝﾄﾞ ﾗｲﾝ上でﾏｸﾛの設定を定義します。例:
!MESSAGE 
!MESSAGE NMAKE /f "dkcCommon.mak" CFG="dkcCommon - Win32 DebugMT"
!MESSAGE 
!MESSAGE 選択可能なﾋﾞﾙﾄﾞ ﾓｰﾄﾞ:
!MESSAGE 
!MESSAGE "dkcCommon - Win32 Release" ("Win32 (x86) Static Library" 用)
!MESSAGE "dkcCommon - Win32 Debug" ("Win32 (x86) Static Library" 用)
!MESSAGE "dkcCommon - Win32 DebugMT" ("Win32 (x86) Static Library" 用)
!MESSAGE "dkcCommon - Win32 ReleaseMT" ("Win32 (x86) Static Library" 用)
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "dkcCommon - Win32 Release"

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

!ELSEIF  "$(CFG)" == "dkcCommon - Win32 Debug"

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
# ADD LIB32 /nologo /out:"Debug\dkcCommond.lib"

!ELSEIF  "$(CFG)" == "dkcCommon - Win32 DebugMT"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "dkcCommon___Win32_DebugMT"
# PROP BASE Intermediate_Dir "dkcCommon___Win32_DebugMT"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "dkcCommon___Win32_DebugMT"
# PROP Intermediate_Dir "dkcCommon___Win32_DebugMT"
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
# ADD LIB32 /nologo /out:"DebugMT\dkcCommonMTd.lib"

!ELSEIF  "$(CFG)" == "dkcCommon - Win32 ReleaseMT"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "dkcCommon___Win32_ReleaseMT"
# PROP BASE Intermediate_Dir "dkcCommon___Win32_ReleaseMT"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "dkcCommon___Win32_ReleaseMT"
# PROP Intermediate_Dir "dkcCommon___Win32_ReleaseMT"
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
# ADD LIB32 /nologo /out:"Release\dkcCommonMT.lib"

!ENDIF 

# Begin Target

# Name "dkcCommon - Win32 Release"
# Name "dkcCommon - Win32 Debug"
# Name "dkcCommon - Win32 DebugMT"
# Name "dkcCommon - Win32 ReleaseMT"
# Begin Group "container"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\dkc2Tree.c
# End Source File
# Begin Source File

SOURCE=.\dkc2Tree.h
# End Source File
# Begin Source File

SOURCE=.\dkcArrayOneByOne.c
# End Source File
# Begin Source File

SOURCE=.\dkcArrayOneByOne.h
# End Source File
# Begin Source File

SOURCE=.\dkcBuffer.c
# End Source File
# Begin Source File

SOURCE=.\dkcBuffer.h
# End Source File
# Begin Source File

SOURCE=.\dkcCircularMemoryStream.c
# End Source File
# Begin Source File

SOURCE=.\dkcCircularMemoryStream.h
# End Source File
# Begin Source File

SOURCE=.\dkcCircularStream.c

!IF  "$(CFG)" == "dkcCommon - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "dkcCommon - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "dkcCommon - Win32 DebugMT"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "dkcCommon - Win32 ReleaseMT"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\dkcCircularStream.h

!IF  "$(CFG)" == "dkcCommon - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "dkcCommon - Win32 Debug"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "dkcCommon - Win32 DebugMT"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "dkcCommon - Win32 ReleaseMT"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\dkcDeque.c
# End Source File
# Begin Source File

SOURCE=.\dkcDeque.h
# End Source File
# Begin Source File

SOURCE=.\dkcMemoryStream.c
# End Source File
# Begin Source File

SOURCE=.\dkcMemoryStream.h
# End Source File
# Begin Source File

SOURCE=.\dkcQueue.c
# End Source File
# Begin Source File

SOURCE=.\dkcQueue.h
# End Source File
# Begin Source File

SOURCE=.\dkcSingleList.c
# End Source File
# Begin Source File

SOURCE=.\dkcSingleList.h
# End Source File
# Begin Source File

SOURCE=.\dkcStack.c
# End Source File
# Begin Source File

SOURCE=.\dkcStack.h
# End Source File
# Begin Source File

SOURCE=.\dkcString.c
# End Source File
# Begin Source File

SOURCE=.\dkcString.h
# End Source File
# End Group
# Begin Group "common"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\dkcDefined.h
# End Source File
# Begin Source File

SOURCE=.\dkcDLL.c
# End Source File
# Begin Source File

SOURCE=.\dkcDLL.h
# End Source File
# Begin Source File

SOURCE=.\dkcLua.c
# End Source File
# Begin Source File

SOURCE=.\dkcLua.h
# End Source File
# Begin Source File

SOURCE=.\dkcMath.c
# End Source File
# Begin Source File

SOURCE=.\dkcMath.h
# End Source File
# Begin Source File

SOURCE=.\dkcMemoryPool.c
# End Source File
# Begin Source File

SOURCE=.\dkcMemoryPool.h
# End Source File
# Begin Source File

SOURCE=.\dkcOS.c
# End Source File
# Begin Source File

SOURCE=.\dkcOS.h
# End Source File
# Begin Source File

SOURCE=.\dkcOSIndependent.c
# End Source File
# Begin Source File

SOURCE=.\dkcOSIndependent.h
# End Source File
# Begin Source File

SOURCE=.\dkcRegex.c
# End Source File
# Begin Source File

SOURCE=.\dkcRegex.h
# End Source File
# Begin Source File

SOURCE=.\dkcSort.c
# End Source File
# Begin Source File

SOURCE=.\dkcSort.h
# End Source File
# Begin Source File

SOURCE=.\dkcStdio.c
# End Source File
# Begin Source File

SOURCE=.\dkcStdio.h
# End Source File
# Begin Source File

SOURCE=.\dkcTaskEngine.c
# End Source File
# Begin Source File

SOURCE=.\dkcTaskEngine.h
# End Source File
# Begin Source File

SOURCE=.\dkcThreadLock.c
# End Source File
# Begin Source File

SOURCE=.\dkcThreadLock.h
# End Source File
# Begin Source File

SOURCE=.\dkcUniqueID.c
# End Source File
# Begin Source File

SOURCE=.\dkcUniqueID.h
# End Source File
# End Group
# Begin Group "filesystem"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\dkcFileSystems.h
# End Source File
# Begin Source File

SOURCE=.\dkcGenericFileSystem.c
# End Source File
# Begin Source File

SOURCE=.\dkcGenericFileSystem.h
# End Source File
# Begin Source File

SOURCE=.\dkcSafeFileSystem.c
# End Source File
# Begin Source File

SOURCE=.\dkcSafeFileSystem.h
# End Source File
# Begin Source File

SOURCE=.\dkcSJISFileSystem.c
# End Source File
# Begin Source File

SOURCE=.\dkcSJISFileSystem.h
# End Source File
# Begin Source File

SOURCE=.\dkcStream.c
# End Source File
# Begin Source File

SOURCE=.\dkcStream.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\dkcLibCommon.h
# End Source File
# End Target
# End Project
