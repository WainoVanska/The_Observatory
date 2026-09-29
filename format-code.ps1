param(
    [string]$ClangFormatPath
)

$ErrorActionPreference = "Stop"

function Find-ClangFormat {
    param(
        [string]$RequestedPath
    )

    if ($RequestedPath) {
        if (-not (Test-Path -LiteralPath $RequestedPath -PathType Leaf)) {
            throw "clang-format was not found at '$RequestedPath'."
        }

        return (Resolve-Path -LiteralPath $RequestedPath).Path
    }

    $command = Get-Command clang-format -ErrorAction SilentlyContinue
    if (-not $command) {
        $command = Get-Command clang-format.exe -ErrorAction SilentlyContinue
    }

    if ($command) {
        return $command.Source
    }

    $vsWhere = Join-Path ${env:ProgramFiles(x86)} "Microsoft Visual Studio\Installer\vswhere.exe"
    if (Test-Path -LiteralPath $vsWhere -PathType Leaf) {
        $installPath = & $vsWhere -latest -products * -property installationPath
        if ($installPath) {
            $visualStudioCandidates = @(
                (Join-Path $installPath "VC\Tools\Llvm\x64\bin\clang-format.exe"),
                (Join-Path $installPath "VC\Tools\Llvm\bin\clang-format.exe")
            )

            foreach ($candidate in $visualStudioCandidates) {
                if (Test-Path -LiteralPath $candidate -PathType Leaf) {
                    return $candidate
                }
            }
        }
    }

    throw "clang-format was not found. Install the Visual Studio C++ Clang tools or add clang-format to PATH."
}

$formatter = Find-ClangFormat -RequestedPath $ClangFormatPath
$sourceRoots = @(
    (Join-Path $PSScriptRoot "The_Observatory\src"),
    (Join-Path $PSScriptRoot "The_Observatory\assets\shaders")
)

$files = foreach ($sourceRoot in $sourceRoots) {
    Get-ChildItem -LiteralPath $sourceRoot -Recurse -File |
        Where-Object { $_.Extension -in ".cpp", ".h", ".glsl" }
}

$files = @($files | Sort-Object FullName)

foreach ($file in $files) {
    & $formatter -i --style=file --fallback-style=none $file.FullName
    if ($LASTEXITCODE -ne 0) {
        throw "clang-format failed for '$($file.FullName)'."
    }
}

Write-Host "Formatted $($files.Count) C++ and GLSL files with '$formatter'."
