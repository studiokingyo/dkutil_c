
echo Y | del html\*.*
echo Y | del latex\*.*
echo Y | del man\*.*
echo Y | del man\man3\*.*
echo Y | del rtf\*.*
echo Y | del xml\*.*
echo Y | del Release\*.*
echo Y | del Release_bcc\*.lib
echo Y | del Release_bcc\*.mak
echo Y | del devcpp\*.a
echo Y | del Makefile.win
echo Y | del ReleaseMT\*.lib
echo Y | del DebugMT\*.lib
call clean.bat
