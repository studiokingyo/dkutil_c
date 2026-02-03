dKingyoVCCleaner.exe -current -outputlog -del0
echo Y | del Debug\*.*

echo Y | del Release_bcc\*.*
echo Y | del Debug_bcc\*.*

echo Y | del devcpp\*.o

echo Y | del *.ncb
echo Y | del *.opt
echo Y | del *.plg
echo Y | del *.log
echo Y | del sample\*.plg
echo Y | del sample\*.log
echo Y | del sample\*.bin
echo Y | del sample\Debug\*.exe
echo Y | del sample\Release\*.exe