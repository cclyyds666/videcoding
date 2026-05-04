param(
    [ValidateSet("Build", "Rebuild", "Clean")]
    [string]$Action = "Build"
)

$ErrorActionPreference = "Stop"

$ProjectRoot = Split-Path -Parent $PSScriptRoot
$ProjectFile = Join-Path $ProjectRoot "Project.uvprojx"
$TargetName = "Target 1"

function Find-Uv4 {
    $cmd = Get-Command "UV4.exe" -ErrorAction SilentlyContinue
    if ($cmd) {
        return $cmd.Source
    }

    $candidates = @(
        "C:\MDK5\UV4\UV4.exe",
        "D:\MDK5\UV4\UV4.exe",
        "C:\Keil_v5\UV4\UV4.exe",
        "D:\Keil_v5\UV4\UV4.exe",
        "C:\Keil\UV4\UV4.exe",
        "D:\Keil\UV4\UV4.exe",
        "C:\keil5\UV4\UV4.exe",
        "D:\keil5\UV4\UV4.exe",
        "D:\天问Block\UV4\UV4.exe",
        "${env:ProgramFiles}\Keil_v5\UV4\UV4.exe",
        "${env:ProgramFiles(x86)}\Keil_v5\UV4\UV4.exe"
    )

    foreach ($candidate in $candidates) {
        if ($candidate -and (Test-Path -LiteralPath $candidate)) {
            return $candidate
        }
    }

    throw "Cannot find UV4.exe. Install Keil MDK-ARM or add Keil\\UV4 to PATH."
}

function Remove-KeilOutput {
    $patterns = @(
        "Objects\*.o", "Objects\*.d", "Objects\*.crf", "Objects\*.axf",
        "Objects\*.dep", "Objects\*.htm", "Objects\*.sct", "Objects\*.lnp",
        "Listings\*.lst", "Listings\*.map"
    )

    foreach ($pattern in $patterns) {
        Get-ChildItem -Path (Join-Path $ProjectRoot $pattern) -ErrorAction SilentlyContinue |
            Remove-Item -Force -ErrorAction SilentlyContinue
    }
}

if (-not (Test-Path -LiteralPath $ProjectFile)) {
    throw "Cannot find project file: $ProjectFile"
}

if ($Action -eq "Clean") {
    Remove-KeilOutput
    Write-Host "Cleaned Keil output files."
    exit 0
}

$uv4 = Find-Uv4
$logFile = Join-Path $ProjectRoot "Objects\vscode-keil-build.log"
New-Item -ItemType Directory -Force (Split-Path -Parent $logFile) | Out-Null

$buildArg = if ($Action -eq "Rebuild") { "-r" } else { "-b" }
$args = @($buildArg, "`"$ProjectFile`"", "-t`"$TargetName`"", "-o`"$logFile`"")

Write-Host "Using Keil: $uv4"
Write-Host "Project: $ProjectFile"
Write-Host "Target: $TargetName"
$process = Start-Process -FilePath $uv4 -ArgumentList $args -Wait -PassThru -WindowStyle Hidden
$exitCode = $process.ExitCode

if (Test-Path -LiteralPath $logFile) {
    Get-Content -LiteralPath $logFile
}

exit $exitCode
