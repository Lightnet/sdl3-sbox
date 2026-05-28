@echo off
setlocal EnableDelayedExpansion

:: =============================================
:: Configuration
:: =============================================
set "VCPKG_ROOT=%~dp0vcpkg"
set "BUILD_DIR=build"
set "GENERATOR=Visual Studio 18 2026"
set "TRIPLET=x64-windows"           :: x64-windows, x64-windows-static, etc.
set "CMAKE_BUILD_TYPE=Release"      :: Release or Debug

echo === Setting up environment ===
echo VCPKG_ROOT: %VCPKG_ROOT%
echo Generator: %GENERATOR%
echo Triplet:   %TRIPLET%
echo Build Type: %CMAKE_BUILD_TYPE%

:: Add vcpkg to PATH (optional but useful)
set "PATH=%VCPKG_ROOT%;%PATH%"

:: Create build directory
if not exist "%BUILD_DIR%" mkdir "%BUILD_DIR%"
pushd "%BUILD_DIR%"

echo.
echo === Configuring with CMake + vcpkg ===
cmake ../ ^
  -G "%GENERATOR%" ^
  -A x64 ^
  -DCMAKE_TOOLCHAIN_FILE="%VCPKG_ROOT%/scripts/buildsystems/vcpkg.cmake" ^
  -DVCPKG_TARGET_TRIPLET=%TRIPLET% ^
  -DVCPKG_INSTALLED_DIR="%~dp0vcpkg_installed" ^
  -DCMAKE_BUILD_TYPE=%CMAKE_BUILD_TYPE%

if %ERRORLEVEL% NEQ 0 (
    echo.
    echo === CMake configuration FAILED ===
    popd
    exit /b 1
)

echo.
echo === Building ===
cmake --build . --config %CMAKE_BUILD_TYPE% -- /m

if %ERRORLEVEL% NEQ 0 (
    echo.
    echo === Build FAILED ===
    popd
    exit /b 1
)

echo.
echo === Build succeeded! ===
popd