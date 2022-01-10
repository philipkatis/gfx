@echo off

rem TODO(philip): Documentation.
rem TODO(philip): Setup compiler flags.

if not exist build (
    mkdir build
)

pushd build

call cl -Z7 ..\code\gfx_win32.c user32.lib

popd
