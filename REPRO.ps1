#Requires -Version 5.1
$ErrorActionPreference = "Stop"
New-Item -ItemType Directory -Force -Path "logs" | Out-Null

# Szukamy platformio.exe / pio.exe z rozszerzenia VS Code
$pio1 = Join-Path $env:USERPROFILE ".platformio\penv\Scripts\platformio.exe"
$pio2 = Join-Path $env:USERPROFILE ".platformio\penv\Scripts\pio.exe"
if (Test-Path $pio1) { $PIO = $pio1 }
elseif (Test-Path $pio2) { $PIO = $pio2 }
else { throw "Nie znaleziono PlatformIO CLI. Otwórz VS Code z PlatformIO lub:  pip install -U platformio" }

# Twoje środowisko z platformio.ini:
$EnvName = "uno_r4_minima"

# Build (pełny log)
$cmd = "`"$PIO`" run -e $EnvName -v"
& cmd /c $cmd *>&1 | Tee-Object -FilePath "logs\build.txt"

# (opcjonalnie) compiledb do analizy
try {
  $cmd2 = "`"$PIO`" run -e $EnvName -t compiledb"
  & cmd /c $cmd2 *>&1 | Tee-Object -FilePath "logs\compiledb.txt"
} catch { Write-Host "compiledb: pomijam (cel niedostępny w tym środowisku)" }

Get-Content "logs\build.txt" -Tail 200
