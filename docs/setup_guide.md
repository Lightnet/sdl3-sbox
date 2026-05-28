

# powershell:
```powershell
Get-ExecutionPolicy -List
```
```powershell
Set-ExecutionPolicy RemoteSigned -Scope LocalMachine
```
```powershell
Set-ExecutionPolicy Restricted -Scope LocalMachine
```

# vcpkg
```powershell
git clone https://github.com/microsoft/vcpkg.git
```

## vcpkg.json
```
{
  "name": "my-game",
  "version": "1.0.0",
  "dependencies": [
    "gamenetworkingsockets",
    {
      "name": "protobuf",
      "default-features": false
    }
  ],
  "default-features": [],
  "overrides": []
}
```

## install.bat
```
@echo off

set "VCPKG_ROOT=%~dp0vcpkg"
set "PATH=%VCPKG_ROOT%;%PATH%"

echo VCPKG_ROOT = %VCPKG_ROOT%
echo PATH updated successfully.

@REM vcpkg install gamenetworkingsockets
vcpkg install
```