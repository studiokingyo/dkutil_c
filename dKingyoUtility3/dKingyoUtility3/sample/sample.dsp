# Microsoft Developer Studio Project File - Name="sample" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** 編集しないでください **

# TARGTYPE "Win32 (x86) Console Application" 0x0103

CFG=sample - Win32 Debug
!MESSAGE これは有効なﾒｲｸﾌｧｲﾙではありません。 このﾌﾟﾛｼﾞｪｸﾄをﾋﾞﾙﾄﾞするためには NMAKE を使用してください。
!MESSAGE [ﾒｲｸﾌｧｲﾙのｴｸｽﾎﾟｰﾄ] ｺﾏﾝﾄﾞを使用して実行してください
!MESSAGE 
!MESSAGE NMAKE /f "sample.mak".
!MESSAGE 
!MESSAGE NMAKE の実行時に構成を指定できます
!MESSAGE ｺﾏﾝﾄﾞ ﾗｲﾝ上でﾏｸﾛの設定を定義します。例:
!MESSAGE 
!MESSAGE NMAKE /f "sample.mak" CFG="sample - Win32 Debug"
!MESSAGE 
!MESSAGE 選択可能なﾋﾞﾙﾄﾞ ﾓｰﾄﾞ:
!MESSAGE 
!MESSAGE "sample - Win32 Release" ("Win32 (x86) Console Application" 用)
!MESSAGE "sample - Win32 Debug" ("Win32 (x86) Console Application" 用)
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "sample - Win32 Release"

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
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /c
# ADD CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /c
# ADD BASE RSC /l 0x411 /d "NDEBUG"
# ADD RSC /l 0x411 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib  kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib  kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386

!ELSEIF  "$(CFG)" == "sample - Win32 Debug"

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
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /GZ  /c
# ADD CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /GZ  /c
# ADD BASE RSC /l 0x411 /d "_DEBUG"
# ADD RSC /l 0x411 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib  kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib  kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386 /pdbtype:sept

!ENDIF 

# Begin Target

# Name "sample - Win32 Release"
# Name "sample - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\sample.c
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\dcf_misc.h
# End Source File
# Begin Source File

SOURCE=.\dkc.h
# End Source File
# Begin Source File

SOURCE=.\dkc2Tree.h
# End Source File
# Begin Source File

SOURCE=.\dkcArcfour.h
# End Source File
# Begin Source File

SOURCE=.\dkcArrayOneByOne.h
# End Source File
# Begin Source File

SOURCE=.\dkcBit.h
# End Source File
# Begin Source File

SOURCE=.\dkcBlockSort.h
# End Source File
# Begin Source File

SOURCE=.\dkcBlowfish.h
# End Source File
# Begin Source File

SOURCE=.\dkcBuffer.h
# End Source File
# Begin Source File

SOURCE=.\dkcBufferedFileStream.h
# End Source File
# Begin Source File

SOURCE=.\dkcCamellia.h
# End Source File
# Begin Source File

SOURCE=.\dkcCircularMemoryStream.h
# End Source File
# Begin Source File

SOURCE=.\dkcCircularStream.h
# End Source File
# Begin Source File

SOURCE=.\dkcCRC.h
# End Source File
# Begin Source File

SOURCE=.\dkcCryptograph.h
# End Source File
# Begin Source File

SOURCE=.\dkcDCF.h
# End Source File
# Begin Source File

SOURCE=.\dkcDCF01.h
# End Source File
# Begin Source File

SOURCE=.\dkcDefined.h
# End Source File
# Begin Source File

SOURCE=.\dkcDeque.h
# End Source File
# Begin Source File

SOURCE=.\dkcDLL.h
# End Source File
# Begin Source File

SOURCE=.\dkcFileSystems.h
# End Source File
# Begin Source File

SOURCE=.\dkcGenericFileSystem.h
# End Source File
# Begin Source File

SOURCE=.\dkcHash.h
# End Source File
# Begin Source File

SOURCE=.\dkcHC256.h
# End Source File
# Begin Source File

SOURCE=.\dkcHMAC.h
# End Source File
# Begin Source File

SOURCE=.\dkcLibCommon.h
# End Source File
# Begin Source File

SOURCE=.\dkcLibCompression.h
# End Source File
# Begin Source File

SOURCE=.\dkcLibCryptograph.h
# End Source File
# Begin Source File

SOURCE=.\dkcLibFileSystem.h
# End Source File
# Begin Source File

SOURCE=.\dkcLua.h
# End Source File
# Begin Source File

SOURCE=.\dkcLZSS.h
# End Source File
# Begin Source File

SOURCE=.\dkcLZW.h
# End Source File
# Begin Source File

SOURCE=.\dkcMath.h
# End Source File
# Begin Source File

SOURCE=.\dkcMD2.h
# End Source File
# Begin Source File

SOURCE=.\dkcMD4.h
# End Source File
# Begin Source File

SOURCE=.\dkcMD5.h
# End Source File
# Begin Source File

SOURCE=.\dkcMemoryPool.h
# End Source File
# Begin Source File

SOURCE=.\dkcMemoryStream.h
# End Source File
# Begin Source File

SOURCE=.\dkcMessageDigest.h
# End Source File
# Begin Source File

SOURCE=.\dkcOS.h
# End Source File
# Begin Source File

SOURCE=.\dkcOSIndependent.h
# End Source File
# Begin Source File

SOURCE=.\dkcQueue.h
# End Source File
# Begin Source File

SOURCE=.\dkcRedBlackTree.h
# End Source File
# Begin Source File

SOURCE=.\dkcRegex.h
# End Source File
# Begin Source File

SOURCE=.\dkcRijndael.h
# End Source File
# Begin Source File

SOURCE=.\dkcRLE.h
# End Source File
# Begin Source File

SOURCE=.\dkcSafeFileSystem.h
# End Source File
# Begin Source File

SOURCE=.\dkcSemiRealRandom.h
# End Source File
# Begin Source File

SOURCE=.\dkcSHA.h
# End Source File
# Begin Source File

SOURCE=.\dkcSHA1.h
# End Source File
# Begin Source File

SOURCE=.\dkcSHA256.h
# End Source File
# Begin Source File

SOURCE=.\dkcSHA384.h
# End Source File
# Begin Source File

SOURCE=.\dkcSHA512.h
# End Source File
# Begin Source File

SOURCE=.\dkcSingleList.h
# End Source File
# Begin Source File

SOURCE=.\dkcSJISFileSystem.h
# End Source File
# Begin Source File

SOURCE=.\dkcSNOW20.h
# End Source File
# Begin Source File

SOURCE=.\dkcSort.h
# End Source File
# Begin Source File

SOURCE=.\dkcStack.h
# End Source File
# Begin Source File

SOURCE=.\dkcStdio.h
# End Source File
# Begin Source File

SOURCE=.\dkcStream.h
# End Source File
# Begin Source File

SOURCE=.\dkcString.h
# End Source File
# Begin Source File

SOURCE=.\dkcTaskEngine.h
# End Source File
# Begin Source File

SOURCE=.\dkcTelecomConfirmFile.h
# End Source File
# Begin Source File

SOURCE=.\dkcTelecomConfirmPacket.h
# End Source File
# Begin Source File

SOURCE=.\dkcThread.h
# End Source File
# Begin Source File

SOURCE=.\dkcThreadLock.h
# End Source File
# Begin Source File

SOURCE=.\dkcUniqueID.h
# End Source File
# Begin Source File

SOURCE=.\dkcUnityArchivers.h
# End Source File
# Begin Source File

SOURCE=.\dkcVernam.h
# End Source File
# Begin Source File

SOURCE=.\md5.h
# End Source File
# Begin Source File

SOURCE=.\md5_vc_mmx.h
# End Source File
# Begin Source File

SOURCE=.\md5_vc_mmx_pd.h
# End Source File
# Begin Source File

SOURCE=.\md_misc.h
# End Source File
# Begin Source File

SOURCE=".\rijndael-alg-fst.h"
# End Source File
# Begin Source File

SOURCE=".\rijndael-api-fst.h"
# End Source File
# Begin Source File

SOURCE=.\snow2tab.h
# End Source File
# Begin Source File

SOURCE=.\st.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# End Group
# End Target
# End Project
