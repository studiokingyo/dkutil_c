call doxy_release.bat
echo Y | del .\lib\*.*
echo nmake dkutil_cDLL.mak CFG="dkutil_cDLL - Win32 Release"
echo nmake dkutil_cDLL.mak CFG="dkutil_cDLL - Win32 ReleaseMT"
dacekeadgar_copy -collectcopy -nosw *.lib .\lib\
dacekeadgar_copy -collectcopy -nosw *.dll .\lib\
