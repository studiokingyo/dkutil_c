echo Y | del temp\*.*
echo Y | del html\*.*
doxygen Doxyfile

echo Y | del html\*.map
echo Y | del html\*.md5
echo Y | del ..\..\doc\dkutil_cstd\*.*
COPY /B html\*.* /B ..\..\doc\dkutil_cstd /B /V
COPY /B html\*.html /B  temp\  /B /V
COPY /B html\*.png /B  temp\   /B /V
COPY /B html\*.css /B  temp\   /B /V



