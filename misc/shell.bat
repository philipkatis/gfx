@echo off

set vswhere="%ProgramFiles(x86)%\Microsoft Visual Studio\Installer\vswhere.exe"

if not exist %vswhere% (
    echo Could not find vswhere! Make sure there is a vaild Visual Studio installation.
    exit /b 1
)

for /f "usebackq tokens=*" %%i in (`%vswhere% -products * -requires Microsoft.VisualStudio.Component.VC.Tools.x86.x64 -latest -property installationPath`) do (
    set VisualStudioDirectory=%%i
)

if not exist "%VisualStudioDirectory%" (
    echo Could not find Visual Studio! Make sure there is a valid Visual Studio installation.
    exit /b 1
)

set vcvarsall="%VisualStudioDirectory%\VC\Auxiliary\Build\vcvarsall.bat"

if not exist %vcvarsall% (
    echo Could not find vcvarsall! Make sure the "Desktop Development with C++" option is included in the Visual Studio Installer.
    exit /b 1
)

call %vcvarsall% x64
