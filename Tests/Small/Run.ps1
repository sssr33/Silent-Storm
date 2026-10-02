param(
    [ValidateSet('Debug', 'Release')]
    [string]$Configuration = 'Debug',
    [ValidatePattern('^[A-Za-z0-9_-]+$')]
    [string]$Test
)

$ErrorActionPreference = 'Stop'
$testsRoot = Split-Path $PSScriptRoot -Parent
$repoRoot = Split-Path $testsRoot -Parent

if ($Test -and -not (Test-Path -LiteralPath (Join-Path $PSScriptRoot "$Test.cpp"))) {
    throw "Unknown small test: $Test"
}

# Keep the test compiler consistent with the primary game's Debug|Win32 project.
[xml]$mainProject = Get-Content -LiteralPath (Join-Path $repoRoot 'Soft\Andy\Jan03\a5dll\Main\Main.vcxproj') -Raw
$debugSettings = @($mainProject.Project.PropertyGroup | Where-Object {
    $_.Condition -match "'Debug\|Win32'" -and $_.PlatformToolset
})
if ($debugSettings.Count -ne 1) {
    throw 'Cannot determine the Main Debug|Win32 PlatformToolset.'
}
$toolset = [string]$debugSettings[0].PlatformToolset

$vswhere = Join-Path ${env:ProgramFiles(x86)} 'Microsoft Visual Studio\Installer\vswhere.exe'
$vsPath = & $vswhere -latest -version '[18.0,19.0)' -products '*' -requires Microsoft.VisualStudio.Component.VC.Tools.x86.x64 -property installationPath
if ($LASTEXITCODE -ne 0 -or -not $vsPath) {
    throw 'Visual Studio 2026 with C++ build tools is required.'
}

$cmake = Join-Path $vsPath 'Common7\IDE\CommonExtensions\Microsoft\CMake\CMake\bin\cmake.exe'
if (-not (Test-Path -LiteralPath $cmake)) {
    $cmake = (Get-Command cmake -ErrorAction Stop).Source
}
$ctest = Join-Path (Split-Path $cmake -Parent) 'ctest.exe'
if (-not (Test-Path -LiteralPath $ctest)) {
    throw "CTest was not found next to $cmake"
}

$buildRoot = Join-Path $testsRoot "__BUILD\Small\$toolset"
Write-Host "Small tests: Win32, $toolset, $Configuration"
& $cmake -S $testsRoot -B $buildRoot -G 'Visual Studio 18 2026' -A Win32 -T $toolset "-DCMAKE_GENERATOR_INSTANCE=$vsPath"
if ($LASTEXITCODE -ne 0) { throw 'Small-test configuration failed.' }

$target = if ($Test) { "Small_$Test" } else { 'SmallTests' }
& $cmake --build $buildRoot --config $Configuration --target $target -- /nologo /verbosity:minimal
if ($LASTEXITCODE -ne 0) { throw 'Small-test compilation failed.' }

$ctestArguments = @('--test-dir', $buildRoot, '-C', $Configuration, '-L', '^small$', '--output-on-failure', '--no-tests=error')
if ($Test) {
    $ctestArguments += @('-R', ('^Small\.' + [regex]::Escape($Test) + '$'))
}
& $ctest @ctestArguments
if ($LASTEXITCODE -ne 0) { throw 'Small tests failed.' }
