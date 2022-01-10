@echo off

rem TODO(philip): Better error messages.
rem TODO(philip): Documentation.

set vswhere="%ProgramFiles(x86)%\Microsoft Visual Studio\Installer\vswhere.exe"

if not exist %vswhere% (
    echo Could not find vswhere!
    exit /b 1
)

for /f "usebackq tokens=*" %%i in (`%vswhere% -prerelease -products * -requires Microsoft.VisualStudio.Component.VC.Tools.x86.x64 -latest -property installationPath`) do (
    set VisualStudioDirectory=%%i
)

if not exist "%VisualStudioDirectory%" (
    echo Could not find Visual Studio!
    exit /b 1
)

set vcvarsall="%VisualStudioDirectory%\VC\Auxiliary\Build\vcvarsall.bat"

if not exist %vcvarsall% (
    echo Could not find vcvarsall!
    exit /b 1
)

call %vcvarsall% x64
