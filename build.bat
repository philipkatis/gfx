@echo off

set CommonCompilerFlags=-nologo -Oi -Gm- -GR- -GS- -EHa- -Gs9999999 -Z7
set CommonLinkerFlags=-incremental:no -opt:ref -nodefaultlib -stack:0x100000,0x100000 -subsystem:windows

set Libraries=libucrt.lib libvcruntime.lib libcmt.lib kernel32.lib user32.lib gdi32.lib opengl32.lib

pushd run_tree

call cl %CommonCompilerFlags% -Od ..\code\gfx_win32.cpp -Fegfx_debug.exe   -link %CommonLinkerFlags% %Libraries%
call cl %CommonCompilerFlags% -O2 ..\code\gfx_win32.cpp -Fegfx_release.exe -link %CommonLinkerFlags% %Libraries%

popd
