# LifeStudio:HEAD 2.5 build dependencies

This directory contains the original SDK files needed by the primary
`Soft/Andy/Jan03/a5dll` source tree. They were retrieved on 2026-10-03 from
[LifeStudio HEAD 2.5 SDK on Internet Archive](https://archive.org/details/lifestudio_head_2_5_sdk).
The uploader describes the package as originating from the Pathologic (2005)
source code; that provenance has not been independently established.
The package's own `ReadMe.txt` identifies it as LifeStudio:HEAD 2.5 SDK,
copyright 2001-2003 LifeMode Interactive, Corp. Original notices are preserved.

## Included files

| Files | Required by |
| --- | --- |
| `Include/LifeStudioHeadAPI.h`, `Include/LifeStudioHeadAPIMMTS.h` | Main's head loading and animation interfaces. |
| `Include/LifeStudioHeadAPIGDP.h`, `Include/LifeStudioHeadAPITransform.h` | LSConverter's GDP interfaces and their transitive include. |
| `Lib/LifeStudioHeadAPI.lib` | Existing Main, Game, MapEdit and LSConverter link inputs. |
| `Lib/GDPFile.lib` | Existing LSConverter link input. |
| `ReadMe.txt` | Original SDK identification and directory description. |

The original files are copied byte-for-byte from the archive's
`LifeStudio HEAD 2.5 SDK/Include`, `Lib` and root directories. Their sizes
and SHA-1 hashes were checked against the
[archive metadata](https://archive.org/metadata/lifestudio_head_2_5_sdk).
`SHA256SUMS` records local SHA-256 hashes. Git attributes preserve these bytes.

## Build integration and limits

`LifeStudioHead.props` supplies repository-relative include and library paths
for every Win32 configuration of Main, Game, MapEdit and LSConverter.
Existing project link inputs select the required import libraries; the property
sheet does not add extra dependencies. No machine-wide SDK settings are needed.

Both import libraries are x86. All six factory symbols in
`LifeStudioHeadAPI.lib` match the exports of the historical
`bin/LifeStudioHeadAPI.dll`. This establishes matching imported symbol names,
not runtime compatibility of the virtual interfaces or serialized data.

Validation on 2026-10-03 with VS 2026 and Win32/v143 checked the SDK include
path on every compilation item in all 16 consumer project configurations.
Main's selected `LSHead.cpp` compilation passed. An isolated compile-and-link
check resolved the `IAnimator`, `IMMTree`, `ISequencer` and `IGDPFile` factories
using this property sheet and both import libraries; its EXE was not run.
The Game Debug build still stopped in Main with 741 reported unrelated errors,
starting with ambiguous `CPtr` comparisons, and no C1083 diagnostics.

The archive's DLLs, editor, samples and large sample assets are not included.
Historical repository runtime DLLs are retained. Game linking, launch and
runtime compatibility need separate verification; this SDK addition does not
resolve unrelated compiler errors or provide x64 support.
