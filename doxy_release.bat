call for_sf_manual.bat

echo Y | del html\*.map
echo Y | del html\*.md5

COPY /B html\*.* /B  ..\doc\dkutil_c\  /B /V
call html_copy.bat

echo Y | del latex\*.*
echo Y | del man\*.*
echo Y | del man\man3\*.*
echo Y | del rtf\*.*
echo Y | del xml\*.*
echo Y | del html\*.*
