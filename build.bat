SET ORIGINAL_DIR=%CD%
CALL "C:\Program Files (x86)\Microsoft Visual Studio\2017\Community\VC\Auxiliary\Build\vcvars64.bat"
cd /D %ORIGINAL_DIR%

cl tests.c tests_rjd.c /Z7 /Gv /W4 -D_CRT_SECURE_NO_WARNINGS
