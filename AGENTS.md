# Repository guide for coding agents

## Goal and scope

The current goal is to migrate Silent Storm to **Visual Studio 2026** and obtain
a reproducible **Win32 build of the game and its required libraries**. Compilation
and linking come first; launching the game and restoring unavailable features
are later milestones. Explicit, limited stubs for obsolete or unavailable
dependencies are acceptable during this stage.

Prefer focused compatibility fixes over engine rewrites, dependency upgrades,
or changes to gameplay and file formats. Keep this guide in English and update
its build-status notes when verified progress changes the baseline.

Preserve the original code and runtime behavior as closely as possible, including
existing bugs, during the compilation migration. Record behavior-related findings
in [BUGS.md](BUGS.md) and defer their fixes until they can be investigated on a
working game. Later fixes must be focused and checked against the original
behavior for gameplay regressions.

## Which source tree to edit

**Use `Soft/Andy/Jan03/a5dll/` as the primary migration baseline.** It contains
the complete A5 solution, 30 converted C++ projects, the CMake wrapper, and all
existing VS 2022 compatibility changes. Resolve source ownership through the
project files and relative includes before editing a similarly named file.

| Path | Role and treatment |
| --- | --- |
| `Soft/Andy/Jan03/a5dll/` | Primary engine, game, libraries, and development tools. Make build and compatibility fixes here. |
| `Soft/Andy/Oct02/src/` | Separate engine snapshot with an old `Main.vcproj` and many overlapping filenames. It is not referenced by the modern A5 projects. Use for comparison, not as a parallel porting target. |
| `Soft/Andy/May03/` | Three later fragments: `GShadowMap.cpp`, `GShadowMap.h`, and a game `Main.cpp`. They differ substantially from the primary tree and are not included in its build. |
| `Soft/Andy/Jun03/` | Four later fragments: `GAnimParticles.cpp/.h` and `Physics.cpp/.h`. They do not form a complete newer engine checkout. |
| `Soft/Andy/Apr03/p2pa7/`, `Soft/Andy/Jan03/pd/` | Separate P2P and particle-dynamics projects, outside the initial game build. |
| `Soft/Monster/` | Separate legacy game/exporter/test projects using `.dsp`/`.dsw`; not referenced by the primary build. |
| `Soft/SDK/stlport/` | Historical STLport sources and project. Existing migration work is moving the main build toward the MSVC standard library. |
| `Soft/SDK/LifeStudioHead-2.5/` | Original SDK headers and x86 import libraries retrieved from Internet Archive, with shared Win32 project settings and recorded provenance. |
| `Tests/` | Maintained repository checks. Short standalone regressions belong in `Tests/Small/`; other categories use separate sibling directories. |
| `Soft/Serialize7/` | Historical VS/VB macro artifacts, not a replacement game solution. The `.vbproj` contains no source-file entries. |
| `Complete/`, `Data/`, `cfg/`, `scripts/` | Game resources, databases, configuration, and scripts. Do not treat them as alternative C++ source roots. |
| `bin/`, `Versions/Current/` | Historical executable/resource distributions. `Current` is an archive label, not evidence of the newest source. |
| `Tools/` | Historical tools, SDK-related files, and runtime binaries. Their presence does not establish compatibility with the new build. |

The primary baseline is **not proven to be the latest historical game revision**.
The May/June fragments contain later changes, but some require missing or moved
interfaces: for example, June's particle code includes `DBFormat/DataPhys.h` and
`wCheckGlassGrenade.h`, neither present in the primary tree. Its `Physics.h` is
only in the June fragment directory. Later fragments also expect
`Misc/Commands.h` and `Misc/LogStream.h`, while the baseline puts those facilities
in `MiscDll/`. Do not copy these files wholesale into the primary tree or choose
a source tree solely from its month name. Port a particular historical change
only after checking all dependent interfaces and serialization differences.

## History and audit baseline

The initial audit was performed on **2026-10-02**, at `11f14fd94`, on
`feature/vs2026_build`. At that point `feature/vs2022_build` pointed to the same
commit, and `main` / `origin/main` pointed to `bfdd464e0`.

Relevant commits:

- `243de3dd3`: imported `Soft/`, including the alternative source snapshots.
  Those snapshots have no separate development history in this repository.
- `63bf8a0e6`: added the 30 `.vcxproj` files, filters, CMake wrapper, ignore rules,
  and `FileIO/Debug.def`.
- `83343a523`: changed 127 C/C++ files in the primary tree; replaced many
  `hash_map` uses with `unordered_map`, adjusted old STL constructs and allocation
  code, and replaced the FMOD implementation with a silent stub.
- `b5c66c788`: removed 40 unavailable Git LFS pointer files, including old
  STLport libraries and several executable/data files.
- `11f14fd94`: ignored the `__BUILD/` directory.

The audit matched **all 127 changed C/C++ files** to `ClCompile` or `ClInclude`
entries in the converted projects. All 61 changed compilation units were listed
without exclusion flags. Across the 30 projects there were 648 compilation
entries, and all referenced source files existed. The changes target the primary
build rather than an unused duplicate tree.

The message of `83343a523` reports progress on MemoryMngrDll, MemoryMngr, Misc,
FileIO, MiscDll, ADOImport, Input, DBFormat, FModSound, and Script, but also lists
Script, Main, and Game as unfinished. Treat that as historical progress, not a
verified passing build. **No fresh compilation or launch was performed during
the initial audit.** Recheck the branch, working tree, and actual build results
at the start of subsequent work.

## Build entry points and toolchain

- `Soft/Andy/Jan03/a5dll/A5.sln` is the **original legacy solution** (format 7.00)
  and still references `.vcproj` files. It does not select the converted projects.
- The sibling `CMakeLists.txt` uses `include_external_msproject` to wrap the
  checked-in `.vcxproj` files. It does not define compilation sources with normal
  CMake targets and requires a Visual Studio generator. Win32 is selected by
  default; unsupported platforms are rejected.
- The converted `.vcxproj` files currently specify **`v143`** and only **Win32**
  configurations. Most have Debug, FastDebug, ReleaseDll, and Release; there are
  a few project-specific exceptions. Start with **Debug|Win32**.
- The wrapper exposes exactly Debug, FastDebug, ReleaseDll, and Release, replacing
  CMake's default configuration list even in an existing cache. AItest maps
  FastDebug to Debug and ReleaseDll to Release because it only has two project
  configurations. A5ExportModel's extra SlowRelease remains project-specific.
- A VS 2026 generator does not by itself retarget the wrapped projects' explicit
  `PlatformToolset` values. Distinguish IDE migration from compiler migration.
- The local audit found Visual Studio Community 2026 18.10.3, installed `v143`
  and `v145` Win32 toolsets, Windows SDKs, and CMake 4.4.3. Discover installed
  versions again rather than hardcoding a machine-specific installation path.
- CMake's `Visual Studio 18 2026` generator requires CMake 4.2 or newer and uses
  `v145` by default. Its [official documentation](https://cmake.org/cmake/help/latest/generator/Visual%20Studio%2018%202026.html)
  is the reference for generator/toolset selection. The wrapper retains a 3.20
  minimum for older Visual Studio generators; use CMake 4.2+ for VS 2026.
- The ignored `a5dll/__BUILD/CMakeCache.txt` found during the audit selected
  VS 2022 and its old installation path. Use a fresh subdirectory for VS 2026;
  do not reuse that cache with a different generator.

The CMake integration was repaired and verified on **2026-10-02**. The generated
VS 2026 entry point is
`Soft/Andy/Jan03/a5dll/__BUILD/vs2026/A5_VCProj_Wrapper.slnx` with the locally
installed CMake 4.4.3. Regenerate it from the wrapper rather than editing it.
Validation covered all 120 external-project configuration mappings, all 101
remaining project references (including GUIDs), and all 162 generated dependency
paths. MSBuild accepted all four solution configurations; the FastDebug
ZERO_CHECK target succeeded. Configuring without `-A` also selected Win32 and
replaced an old default configuration list correctly. **This verifies generation
and build metadata, not C++ compilation or game linking.**

Preserve the old solution and `.vcproj` files as references unless the task
explicitly calls for removing them. Keep project dependencies in one consistent
graph; inspect `git diff` after conversion or generation so generated metadata
does not get mixed into sources.

## Verified compilation status (through 2026-10-03)

The generated VS 2026 `.slnx` was built with MSBuild, **Debug|Win32**, and the
projects' existing **v143** compiler (14.44.35207). Incremental dependency builds
passed for MemoryMngrDll, MemoryMngr, Misc, FileIO, ADOImport, MiscDll, Input,
DBFormat, FModSound, and Script. This is not a clean-build verification.
**Main still fails compilation; Game has not reached compilation or linking.**

The syntax compatibility batch fixes 293 active `CDynamicCast` declarations in
conditions by using brace initialization, qualifies member-function pointers,
includes complete player-event types where registration templates need them,
and gives `ZSHIFT` its explicit `int` type. These are syntax/declaration changes
in the primary Main project; preserve the legacy source encodings.

MSBuild reported 1911 errors before this batch and 888 after the broad syntax
pass. These counts include cascades, not independent defects. A final missed
conditional declaration in `wUnitAttack.cpp` was fixed and checked with a
selected-file compilation: only C2280 for `SFBTransform` remained in that file.
Logs are ignored local artifacts in `a5dll/__BUILD/vs2026/`:
`game-debug-build-1.log`, `game-debug-build-2.log`, and
`game-debug-wUnitAttack.log`.

The subsequent matrix compatibility fix adds a `SHMatrix()` constructor that
explicitly initializes all 16 scalar elements to zero. `SFBTransform` retains
its implicit constructor, which now constructs both matrices successfully.
Ordinary declarations and brace initialization both produce zero matrices;
call `Identity()` explicitly when an identity matrix is required. The Win32/v143
runtime probe against the edited header passed 22 checks for initialization,
arrays, template members, copying, and basic matrix operations. Static checks
confirmed 64-byte matrices, 128-byte transforms, 4-byte alignment, and preserved
standard layout and trivial copying. The maintained check now lives in
`Tests/Small/MatrixInitialization.cpp`; generated test outputs are ignored in
`Tests/__BUILD/`.
A repeated Game Debug|Win32 build reported 741 errors, with no remaining C2280
diagnostics for `SHMatrix` or `SFBTransform`. Main still fails on other issues;
see the ignored `a5dll/__BUILD/vs2026/game-debug-matrix-constructor.log`.

The LifeStudio SDK was restored on **2026-10-03**. All compilation items in
all 16 configurations of Main, Game, MapEdit and LSConverter resolve the shared
SDK include path. The SDK file hashes, transitive includes and inherited library
paths were checked. A selected-file MSBuild `ClCompile` of Main's `LSHead.cpp`
passed on Debug|Win32/v143 with zero errors. The old C1083 missing-header blocker
is resolved; this selected-file result is not a full Main/Game build or runtime
verification. The log is `a5dll/__BUILD/vs2026/main-debug-lifestudio-sdk.log`.
An isolated Win32/v143 compile-and-link check also resolved `IAnimator`,
`IMMTree`, `ISequencer` and `IGDPFile` factory imports through the shared
property sheet and both original import libraries. Its EXE was not run.
The subsequent generated-solution Game Debug|Win32/v143 build still failed in
Main with **741 reported errors**, beginning with C2666 ambiguous `CPtr`
comparisons in the MSVC STL. There were no C1083 missing-header diagnostics
or errors naming the LifeStudio/GDP SDK. Game compilation/linking was not
reached. See `a5dll/__BUILD/vs2026/game-debug-lifestudio-sdk.log`;
unrelated compiler fixes remain outside the SDK restoration batch.

The next work needs separate, focused investigation:
- **C2666 and other STL/type errors:** `CPtr` comparisons are ambiguous in the
  MSVC standard library; legacy container insertion calls and incomplete or
  missing types also remain. Do not alter pointer ownership to silence errors.
- **Other C2280 diagnostics:** `NAI::SMove` in `Main/aiPosition.h` still has
  deleted default construction and copy assignment involving union members.
  These errors were also present before the matrix-constructor change.
- **Existing inverse status bug:** `SHMatrix::HomogeneousInverse()` returns
  `false` even after successfully computing an inverse. Matrix-result checks
  pass; the return-value defect is separate from the constructor fix. Preserve
  this behavior during the compilation migration: changing the result affects
  debris inertia selection. See [BUG-001 in BUGS.md](BUGS.md#bug-001-homogeneousinverse-reports-failure-after-successful-inversion)
  for the source evidence and deferred runtime validation.

## Known build issues to address

These are static audit findings, not a complete compiler-error inventory:

The stale `..\!!!BUILD\ZERO_CHECK.vcxproj` references were removed from all 30
converted projects. CMake supplies the regeneration dependencies in the generated
solution, using its actual build directory. Do not reintroduce references to
generated projects inside the checked-in `.vcxproj` files.

1. **DLL exports:** many projects specify `$(Configuration).def`, but most lack
   `Debug.def`. Only `FileIO/Debug.def` is tracked; some ignored build outputs may
   exist locally. Check `.def` handling, symbol decoration, and export/import
   declarations when linking; do not assume an old `.def` fits a new compiler.
2. **FMOD:** `FModSound/FMSound.cpp` already contains the silent `NFMSound` stub.
   Initialization reports false and resource/playback functions return null.
   Nevertheless, FModSound and Game still link `fmodvc.lib`. Keep the stub's
   API consistent and remove the library requirement in stub configurations.
   `NSound::InitSound` currently calls `SearchDevices`, so the stub lets that
   initial check pass. Later sound-mode setup calls `NFMSound::Init`, which fails
   when sound is enabled. Review `SetModeFromConfig` and null-result callers
   during runtime work; successful linking does not restore audio.
3. **LifeStudio:** Main's `LSHead.h` includes `LifeStudioHeadAPI.h` and
   `LifeStudioHeadAPIMMTS.h`; Main and Game link `lifeStudioHeadAPI.lib`.
   The original SDK headers and x86 import libraries are now retained in
   `Soft/SDK/LifeStudioHead-2.5/`. Main, Game, MapEdit and LSConverter import its
   `LifeStudioHead.props` for all Win32 configurations. LSConverter also uses
   the GDP header and `GDPFile.lib`. Runtime interface/data compatibility remains
   unverified; matching factory exports alone does not establish it.
4. **Graphics:** the primary renderer includes `D3D9.h` and links `d3d9.lib`;
   Game also links DirectInput 8. README references and startup error messages
   mentioning DirectX 8 do not describe all current source requirements. Check
   actual includes/link inputs before installing or replacing an SDK. Bink DLLs
   exist in the runtime archives, but no Bink references were found in the primary
   C++ projects during the audit.
5. **Shader generation:** Main retains a custom command pointing to
   `w:\tools\ShaderCompiler`. It is currently excluded in all four configurations,
   and `Main/GfxShaders.h/.cpp` are tracked. Keep those sources usable; if shader
   regeneration is needed, build/use a repository-local tool and correct its
   output paths before enabling the command.
6. **ADO:** `ADOImport/BasicDB.cpp` imports `msado15.dll` through an absolute
   `C:\Program Files\Common Files\System\ADO\` path. Validate COM/type-library
   availability and architecture; do not confuse this with the proprietary SDKs.
7. **Partial STL migration:** some projects, including Game, still use
   `<hash_map>` and STLport configuration includes. Review hash/equality functors,
   old container insertion APIs, precompiled headers, and the `#define for`
   workaround where compiler errors point to them. Preserve object ownership and
   allocator behavior when adjusting the standard-library boundary.
8. **Configurations and outputs:** the projects mix static/DLL configurations and
   write to `a5dll/Binary/$(Configuration)` with intermediates in project-local
   configuration folders. Match CRT and import/export settings across libraries.
   The current ignore rules cover `__BUILD/`, `Binary/`, `Debug/`, and `.vs/`;
   check ignores before using other configurations.

## LifeStudio sources and external reference projects

The user explicitly requested the SDK addition on **2026-10-03**. The source is
[LifeStudio HEAD 2.5 SDK on Internet Archive](https://archive.org/details/lifestudio_head_2_5_sdk).
Its uploader describes it as coming from the Pathologic (2005) source code;
treat that as the uploader's statement, not independently verified provenance.
The package includes the original LifeMode headers, x86 import libraries,
runtime DLLs, programmer documentation and samples. Only the current build's
required headers/import libraries and the vendor ReadMe are retained here.
See [the SDK README](Soft/SDK/LifeStudioHead-2.5/README.md) for contents, hashes,
build integration and compatibility limits. Preserve the original files and
copyright notices; do not replace the historical runtime DLLs as a side effect.

Two external projects were inspected on **2026-10-03** and show substantial
progress beyond this repository's compilation milestone:

- [mrartanis/Silent-Storm-Reconstruction](https://github.com/mrartanis/Silent-Storm-Reconstruction/tree/develop/third_party/lifestudio),
  inspected at `e783d5d1603a091b98b452bb1976c187c7394f97`: its code and reports
  cover Windows x64 and Linux SDL3/bgfx builds, native LifeStudio animation and
  FaceGen, and comparisons against the original x86 DLL. Reported tests include
  all 136 game head streams with selected sequences and all 6,780 exported
  sequences at sampled times for one saved custom head. This is bounded evidence,
  not complete game/visual parity or a verified campaign playthrough.
- [TanghaohanSC/silent-storm-port](https://github.com/TanghaohanSC/silent-storm-port/tree/main/src/stubs/lshead),
  inspected at `57b49ef544d21385221bf54f27d001ba56b74c73`: its code and reports
  cover Windows x86 mission rendering, SDL3/bgfx, miniaudio and RmlUi. Its
  acceptance reports retain movement, AI, destruction and UI simplifications.
  LifeStudio uses reconstructed declarations and the original DLL, not a native
  replacement; those declarations differ from the SDK retained here.

These are reference projects only. **Do not copy their implementations,
reconstructed headers, stubs, gameplay changes or platform/renderer migrations
into this repository at this stage.** The user explicitly chose the original
SDK and preservation of the original code. Study external results as evidence
and leads for later investigation; adopting their code requires a separate
explicit user request and compatibility review.

## Focused repository checks

Keep maintained checks under `Tests/`, not inside ignored game build directories.
Use `Tests/Small/` for short, standalone checks without game data or proprietary
SDK requirements. Add separate sibling categories for other work, such as
`Tests/Integration/` or `Tests/Performance/`, when those checks are introduced.
See `Tests/README.md` for the layout, requirements, and how to add a check.

Run `Tests/Small/Run.ps1` from PowerShell. It configures the independent test
CMake project for VS 2026/Win32 using Main's Debug|Win32 `PlatformToolset`, builds
the small-test targets, and runs their CTest label. Use `-Test MatrixInitialization`
for one check and `-Configuration Release` for the optimized configuration.
The existing `__BUILD/` ignore rule covers all generated test projects and logs.
Small-test success does not imply that Main/Game compile, link, or launch.
The maintained matrix check passed all 22 runtime checks in Debug and Release
on Win32/v143 on 2026-10-02, including a selected-test run from another directory.

## Workflow for subsequent migration work

1. Check `git status`, the current branch, and relevant commit history. Preserve
   the user's work and do not reset either source snapshot or build changes.
2. Verify that the intended solution/project builds the primary source tree.
   Fix the project graph and choose an installed toolset/SDK before diagnosing
   missing-library or language errors.
3. Build the game dependency graph incrementally: MemoryMngrDll -> MemoryMngr;
   then Misc -> FileIO -> MiscDll/ADOImport/Input; then DBFormat/FModSound/Script;
   then Main -> Game. Consult actual project references for the precise order.
   The editor, exporters, and unrelated utilities can be handled after this graph.
4. Prefer reproducible repository settings over machine-wide include/library
   paths. Apply compatible changes consistently across affected projects and
   configurations, while keeping the first supported configuration explicit.
5. Isolate dependency stubs behind the existing subsystem API and an explicit
   build setting when practical. Document disabled behavior and any remaining
   startup assumptions. Do not stub gameplay, serialization, or core memory
   management merely to suppress diagnostics.
6. Record the exact command, toolset, configuration, successful targets, and
   remaining first meaningful compiler/linker errors. Distinguish generation,
   compilation, linking, and runtime verification in reports.

Example commands from the repository root:

```powershell
# Inspect the migration and source ownership.
git status --short
git log --oneline -8
git show --stat 83343a523
rg -n 'ClCompile|ClInclude|ProjectReference' Soft/Andy/Jan03/a5dll/Game/Game.vcxproj

# Generate a separate VS 2026 wrapper solution; external projects retain their
# own PlatformToolset until those project properties are explicitly migrated.
cmake -S Soft/Andy/Jan03/a5dll -B Soft/Andy/Jan03/a5dll/__BUILD/vs2026 -G "Visual Studio 18 2026" -A Win32
cmake --build Soft/Andy/Jan03/a5dll/__BUILD/vs2026 --config Debug --target Game

# Reproduce the logged Debug game build with the installed MSBuild.
$vswherePath = Join-Path ${env:ProgramFiles(x86)} 'Microsoft Visual Studio\Installer\vswhere.exe'
$vsPath = & $vswherePath -latest -products '*' -property installationPath
$msbuildPath = Join-Path $vsPath 'MSBuild\Current\Bin\MSBuild.exe'
$solutionPath = (Resolve-Path 'Soft/Andy/Jan03/a5dll/__BUILD/vs2026/A5_VCProj_Wrapper.slnx').Path
$buildLog = Join-Path (Split-Path $solutionPath -Parent) 'game-debug-build.log'
& $msbuildPath $solutionPath '/t:Game' '/p:Configuration=Debug;Platform=Win32' '/m:1' '/nologo' '/noconsolelogger' '/fl' "/flp:LogFile=$buildLog;Verbosity=normal;Encoding=UTF-8"
```

Do not default to building every wrapper target: the exporter/editor/tool projects
bring additional historical dependencies unrelated to the first game build.
Keep x64 migration separate; the present project settings and old code assume a
32-bit target. For documentation-only changes, validate paths, facts, and the diff
without rebuilding the game.

## Editing and repository hygiene

- Search with `rg` within the primary tree first. Broaden to archive snapshots
  deliberately, and identify the owning project before editing duplicates.
- Preserve existing source formatting, line endings, and encoding. Many files
  contain legacy Russian comments; avoid whole-file encoding conversions or
  broad reformatting as a side effect of compatibility work.
- Preserve serialization field IDs (`ZDATA`, `ZEND`, `CStructureSaver`), object
  registration, `CObj`/`CPtr` ownership semantics, and DLL boundaries.
- Do not commit generated solutions, caches, object files, or binaries from
  ignored build folders. A deliberately maintained modern solution can be
  committed when it becomes the chosen build entry point.
- Inspect `.gitattributes` and LFS availability before depending on large
  resources. Some entries still refer to files removed by `b5c66c788`; missing
  archived data should not be treated as a C++ source error.
- Preserve `LICENSE.md` and third-party notices. Do not add proprietary SDKs or
  commercial dependencies as an incidental compiler fix.
