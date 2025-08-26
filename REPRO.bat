@echo off
setlocal
if not exist logs mkdir logs
set "PIO=%USERPROFILE%\.platformio\penv\Scripts\platformio.exe"
if not exist "%PIO%" set "PIO=%USERPROFILE%\.platformio\penv\Scripts\pio.exe"
if not exist "%PIO%" (
  echo Nie znaleziono PlatformIO CLI (zainstaluj rozszerzenie VS Code lub pip install platformio)
  exit /b 1
)
"%PIO%" run -e uno_r4_minima -v > logs\build.txt 2>&1
"%PIO%" run -e uno_r4_minima -t compiledb > logs\compiledb.txt 2>&1
type logs\build.txt
