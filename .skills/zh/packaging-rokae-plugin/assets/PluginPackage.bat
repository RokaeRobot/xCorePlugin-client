@echo off
setlocal EnableExtensions EnableDelayedExpansion

REM Get script directory (remove trailing backslash)
set "SCRIPT_DIR=%~dp0"
if "%SCRIPT_DIR:~-1%"=="\" set "SCRIPT_DIR=%SCRIPT_DIR:~0,-1%"

REM Get current folder name
for %%I in ("%SCRIPT_DIR%") do set "CURRENT_NAME=%%~nxI"

set "CLIENT_DIR=%SCRIPT_DIR%\client"
set "CONTROLLER_DIR=%SCRIPT_DIR%\controller"
set "TEMP_DIR=%TEMP%\zip_work_%RANDOM%"
set "CLIENT_ZIP=%TEMP_DIR%\client.zip"
set "CONTROLLER_ZIP=%TEMP_DIR%\controller.zip"
set "FINAL_ZIP=%SCRIPT_DIR%\%CURRENT_NAME%.zip"

echo.
echo ========================================
echo  Zip Tool
echo ========================================
echo.
echo Script: %SCRIPT_DIR%
echo Output: %FINAL_ZIP%
echo.

REM Check if both folders are missing or empty
echo [1/5] Checking folders...

set "CLIENT_EXISTS=0"
set "CONTROLLER_EXISTS=0"

if exist "%CLIENT_DIR%\" (
  for /f %%a in ('dir /b "%CLIENT_DIR%\" 2^>nul') do (
    set "CLIENT_EXISTS=1"
    goto :check_controller
  )
)
:check_controller
if exist "%CONTROLLER_DIR%\" (
  for /f %%a in ('dir /b "%CONTROLLER_DIR%\" 2^>nul') do (
    set "CONTROLLER_EXISTS=1"
    goto :check_done
  )
)
:check_done

if %CLIENT_EXISTS%==0 if %CONTROLLER_EXISTS%==0 (
  echo [Error] Both folders are empty or missing
  pause >nul
  exit /b 1
)

echo [2/5] Initializing...
if exist "%TEMP_DIR%" rmdir /s /q "%TEMP_DIR%" 2>nul
if exist "%FINAL_ZIP%" del /f /q "%FINAL_ZIP%" 2>nul
mkdir "%TEMP_DIR%" 2>nul

REM Build PowerShell command for final archive
set "ARCHIVE_LIST="

if %CLIENT_EXISTS%==1 (
  echo [3/5] Compressing client...
  cd /d "%CLIENT_DIR%" 2>nul
  powershell -NoLogo -NoProfile -ExecutionPolicy Bypass -Command "Get-ChildItem | Compress-Archive -DestinationPath '%CLIENT_ZIP%' -Force"
  cd /d "%SCRIPT_DIR%" 2>nul
  set "ARCHIVE_LIST=%CLIENT_ZIP%"
)

if %CONTROLLER_EXISTS%==1 (
  echo [4/5] Compressing controller...
  cd /d "%CONTROLLER_DIR%" 2>nul
  powershell -NoLogo -NoProfile -ExecutionPolicy Bypass -Command "Get-ChildItem | Compress-Archive -DestinationPath '%CONTROLLER_ZIP%' -Force"
  cd /d "%SCRIPT_DIR%" 2>nul
  if "%ARCHIVE_LIST%"=="" (
    set "ARCHIVE_LIST=%CONTROLLER_ZIP%"
  ) else (
    set "ARCHIVE_LIST=%ARCHIVE_LIST%','%CONTROLLER_ZIP%"
  )
)

echo [5/5] Creating final archive...
cd /d "%TEMP_DIR%" 2>nul
powershell -NoLogo -NoProfile -ExecutionPolicy Bypass -Command "Compress-Archive -Path '%ARCHIVE_LIST%' -DestinationPath '%FINAL_ZIP%' -Force"
cd /d "%SCRIPT_DIR%" 2>nul

echo [6/5] Cleaning up...
if exist "%TEMP_DIR%" rmdir /s /q "%TEMP_DIR%" 2>nul

echo.
echo [OK] Done: %FINAL_ZIP%
echo.
echo Press any key to exit...
pause >nul
exit /b 0
