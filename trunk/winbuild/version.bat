@for /F "tokens=1* delims== " %%i in (..\configure) do @call :oneline "%%i" "%%j"
@if "%mnt%"=="NO" goto :writereleaseversion
@goto :writecvsversion
 
:oneline
@if "-%~1"=="-doxygen_version_major" call :aset major %2 
@if "-%~1"=="-doxygen_version_minor" call :aset minor %2 
@if "-%~1"=="-doxygen_version_revision" call :aset revision %2 
@if "-%~1"=="-doxygen_version_mmn" call :aset mnt %2 
@goto :EOF

:aset
@set %1=%~2
@goto :EOF

:writecvsversion
echo %major%.%minor%.%revision%-%mnt%>..\VERSION
echo char versionString[]="%major%.%minor%.%revision%-%mnt%";>..\src\version.cpp
@goto :EOF

:writereleaseversion
echo %major%.%minor%.%revision%>..\VERSION
echo char versionString[]="%major%.%minor%.%revision%";>..\src\version.cpp
@goto :EOF
