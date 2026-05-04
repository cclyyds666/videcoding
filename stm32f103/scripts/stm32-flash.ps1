param(
    [int]$Freq = 1000,
    [switch]$SkipBuild
)

$ErrorActionPreference = "Stop"

$ProjectRoot = Split-Path -Parent $PSScriptRoot
$AxfFile = Join-Path $ProjectRoot "Objects\Project.axf"
$HexFile = Join-Path $ProjectRoot "Objects\Project.hex"
$BuildScript = Join-Path $PSScriptRoot "keil-build.ps1"

function Find-FromElf {
    $cmd = Get-Command "fromelf.exe" -ErrorAction SilentlyContinue
    if ($cmd) {
        return $cmd.Source
    }

    $candidates = @(
        "D:\MDK5\ARM\ARMCC\bin\fromelf.exe",
        "C:\MDK5\ARM\ARMCC\bin\fromelf.exe",
        "D:\Keil_v5\ARM\ARMCC\bin\fromelf.exe",
        "C:\Keil_v5\ARM\ARMCC\bin\fromelf.exe"
    )

    foreach ($candidate in $candidates) {
        if ($candidate -and (Test-Path -LiteralPath $candidate)) {
            return $candidate
        }
    }

    throw "Cannot find fromelf.exe. Install Keil MDK-ARM or add ARMCC\bin to PATH."
}

function Find-OpenOcd {
    $cmd = Get-Command "openocd.exe" -ErrorAction SilentlyContinue
    if ($cmd) {
        return @{
            Exe = $cmd.Source
            Scripts = $null
        }
    }

    $candidates = @(
        @{
            Exe = "D:\GD32IDE\MCU-GD32 Embedded Builder\GD32EmbeddedBuilder_v1.5.4_Rel\Tools\OpenOCD\xpack-openocd-0.11.0-3\bin\openocd.exe"
            Scripts = "D:\GD32IDE\MCU-GD32 Embedded Builder\GD32EmbeddedBuilder_v1.5.4_Rel\Tools\OpenOCD\xpack-openocd-0.11.0-3\scripts"
        },
        @{
            Exe = "D:\ESP32\Espressif_Tools\tools\openocd-esp32\v0.12.0-esp32-20250707\openocd-esp32\bin\openocd.exe"
            Scripts = "D:\ESP32\Espressif_Tools\tools\openocd-esp32\v0.12.0-esp32-20250707\openocd-esp32\share\openocd\scripts"
        }
    )

    foreach ($candidate in $candidates) {
        if ($candidate.Exe -and (Test-Path -LiteralPath $candidate.Exe)) {
            return $candidate
        }
    }

    throw "Cannot find openocd.exe. Install OpenOCD or add it to PATH."
}

if (-not $SkipBuild) {
    & powershell -NoProfile -ExecutionPolicy Bypass -File $BuildScript -Action Build
    if ($LASTEXITCODE -ne 0) {
        exit $LASTEXITCODE
    }
}

if (-not (Test-Path -LiteralPath $AxfFile)) {
    throw "Cannot find build output: $AxfFile"
}

$fromelf = Find-FromElf
Write-Host "Converting AXF to HEX: $HexFile"
& $fromelf --i32 --output="$HexFile" "$AxfFile"
if ($LASTEXITCODE -ne 0) {
    exit $LASTEXITCODE
}

$openocd = Find-OpenOcd
$hexForOpenOcd = $HexFile -replace "\\", "/"

Write-Host "Flashing with OpenOCD: $($openocd.Exe)"
$openocdArgs = @()
if ($openocd.Scripts) {
    $openocdArgs += @("-s", $openocd.Scripts)
}
$openocdArgs += @(
    "-f", "interface/stlink.cfg",
    "-f", "target/stm32f1x.cfg",
    "-c", "adapter speed $Freq",
    "-c", "program $hexForOpenOcd verify reset exit"
)

& $openocd.Exe @openocdArgs
exit $LASTEXITCODE
