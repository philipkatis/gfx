@echo off

rem TODO(philip): Documentation.
rem TODO(philip): Setup compiler flags.

if not exist build (
    mkdir build
)

pushd build

call cl -Z7 ..\code\gfx_win32.cpp user32.lib gdi32.lib opengl32.lib

popd
