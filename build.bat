@echo off

rem TODO(philip): Documentation.
rem TODO(philip): Setup compiler flags.

if not exist build (
    mkdir build
)

pushd build

call cl ..\code\gfx_win32.c

popd
