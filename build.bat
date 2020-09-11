rem SET ORIGINAL_DIR=%CD%
rem CALL "C:\Program Files (x86)\Microsoft Visual Studio\2017\Community\VC\Auxiliary\Build\vcvars64.bat"
rem cd /D %ORIGINAL_DIR%

REM /Z7 old debug info format - don't use pdb
REM /Gv __vectorcall calling convention
REM /W4 warning level 4
REM /Oi use intrinsics
REM -D_CRT_SECURE_NO_WARNINGS disable msvc-specific "unsafe" function warninings

cl tests.c tests_rjd.c /Z7 /Gv /W4 /Oi -D_CRT_SECURE_NO_WARNINGS /link /MACHINE:X64 shell32.lib user32.lib
