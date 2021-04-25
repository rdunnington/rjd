@REM /Z7 old debug info format - don't use pdb
@REM /Gv __vectorcall calling convention
@REM /W4 warning level 4
@REM /Oi use intrinsics
@REM /D_CRT_SECURE_NO_WARNINGS disable msvc-specific "unsafe" function warninings
@REM /DWINVER=_WIN32_WINNT_WIN10 /D_WIN32_WINNT=_WIN32_WINNT_WIN10 /DWDK_NTDDI_VERSION=NTDDI_WIN10_RS1 windows 10 SDK

SET SOURCES=tests.c tests_rjd.c
SET CFLAGS=/Z7 /Gv /W4 /Oi /std:c11
SET DEFINES=/D_CRT_SECURE_NO_WARNINGS /DWINVER=_WIN32_WINNT_WIN10 /D_WIN32_WINNT=_WIN32_WINNT_WIN10 
SET LFLAGS=/MACHINE:X64
SET LIBS=shell32.lib user32.lib dxgi.lib d3dcompiler.lib d3d11.lib dxguid.lib gdi32.lib

cl %SOURCES% %CFLAGS% %DEFINES% /link %LFLAGS% %LIBS% /OUT:tests.exe
