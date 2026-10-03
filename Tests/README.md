# Repository checks

Keep maintained checks here, separate from the historical game/tool projects.
The test CMake project is independent of the A5 wrapper: the small checks can
compile and run while Main/Game still have unresolved dependencies.

## Structure

- `Small/`: short, focused checks for individual types, helpers, or compatibility
  fixes. Each `.cpp` builds a standalone executable and finishes within 30 seconds.
- `__BUILD/`: generated projects, executables, and CTest logs; ignored by Git.

Create a separate sibling directory when a different kind of check is needed,
for example `Integration/` for multiple subsystems or `Performance/` for
benchmarks. Give that directory its own build/run instructions. Keep checks
requiring game resources, proprietary SDKs, services, or long runs out of Small.

## Run the small checks

Requirements: Windows, Visual Studio 2026 with the Win32 toolset used by Main,
a Windows SDK, and CMake 4.2+ with CTest. The script prefers Visual Studio's
bundled CMake and reads `PlatformToolset` from Main's Debug|Win32 configuration.
The build uses a static CRT so the test executables do not require debug CRT DLLs.

From the repository root in PowerShell:

```powershell
.\Tests\Small\Run.ps1
.\Tests\Small\Run.ps1 -Test MatrixInitialization
.\Tests\Small\Run.ps1 -Test PointerComparison
.\Tests\Small\Run.ps1 -Configuration Release
```

The script can also be invoked by absolute path from another working directory.
Generated files are stored in `Tests/__BUILD/Small/<toolset>/`. It builds only the
requested small-test target(s), runs the `small` CTest label, and fails when
configuration, compilation, or a check fails.

To see successful test output as well as failures:

```powershell
ctest --test-dir Tests/__BUILD/Small/v143 -C Debug -L small -V
```

## Maintained checks

`Small/MatrixInitialization.cpp` includes the actual primary-tree `Misc/Geom.h`.
Its 22 runtime checks cover zero initialization of SHMatrix/SFBTransform,
arrays, a DG-style value member, copying, explicit identity setup, translation,
multiplication, and inverse matrix results. Compile-time checks protect Win32
size/alignment, matrix offsets, default construction, and copy/layout traits.

The inverse check validates the calculated matrix, not the return flag:
`SHMatrix::HomogeneousInverse()` currently returns false even on success. That
existing defect remains separate from the matrix-constructor fix.

`Small/PointerComparison.cpp` includes the primary-tree `Misc/Basic2.h` and
links the original `Misc/Basic2.cpp` reference-counting runtime. Its 91 checks
cover CPtr/CObj/CMObj address equality and inequality, const/raw pointers,
zero/NULL/nullptr, mutable and const STL searches, container removal, unchanged
reference/owner counts, invalidated weak references and last-reference deletion.
Compile-time checks protect pointer-sized layout and implicit pointer conversion.
The check reproduced C2666 before the fix and passes on Win32/v143 in both
Debug and Release. It requires neither game data nor proprietary dependencies.

## Add a small check

Add a descriptively named `.cpp` to `Small/` with its own `main()`. CMake discovers
these files automatically. Return zero only when all checks pass; print a useful
failure description and return nonzero otherwise. CMake registers the executable
as `Small.<filename>` with CTest and adds it to the `SmallTests` build target.
Use the actual migration sources rather than copied or simplified definitions.
