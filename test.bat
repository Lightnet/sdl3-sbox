@echo off
echo The active working directory is: %CD%
set "VCPKG_ROOT=%~dp0vcpkg"
@REM echo "dir: %~dp0"
echo VCPKG_ROOT = %VCPKG_ROOT%
pause