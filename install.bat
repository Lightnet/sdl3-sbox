@echo off

set "VCPKG_ROOT=%~dp0vcpkg"
set "PATH=%VCPKG_ROOT%;%PATH%"

echo VCPKG_ROOT = %VCPKG_ROOT%
echo PATH updated successfully.

@REM vcpkg install gamenetworkingsockets
vcpkg install