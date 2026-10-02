cmake_minimum_required(VERSION 3.20)

# Wrap the converted .vcxproj files while preserving their build settings.
# These external projects only support the Win32 platform.
if(NOT CMAKE_GENERATOR MATCHES "^Visual Studio ")
  message(FATAL_ERROR "The A5 wrapper requires a Visual Studio generator.")
endif()

if(NOT CMAKE_GENERATOR_PLATFORM)
  set(CMAKE_GENERATOR_PLATFORM Win32 CACHE STRING "A5 target platform" FORCE)
elseif(NOT CMAKE_GENERATOR_PLATFORM STREQUAL "Win32")
  message(FATAL_ERROR "The A5 projects only support Win32. Configure with -A Win32.")
endif()

# Replace CMake's default configurations, including values in an existing cache.
set(CMAKE_CONFIGURATION_TYPES "Debug;FastDebug;ReleaseDll;Release"
    CACHE STRING "Configurations supported by the A5 projects" FORCE)

project(A5_VCProj_Wrapper NONE)

function(a5_add_vcproj target relpath)
  include_external_msproject(${target} "${CMAKE_CURRENT_LIST_DIR}/${relpath}"
                            PLATFORM Win32)
endfunction()

# Projects from A5.sln
a5_add_vcproj(A5ExportModel "A5ExportModel/A5ExportModel.vcxproj")
a5_add_vcproj(DBFormat "DBFormat/DBFormat.vcxproj")
a5_add_vcproj(DataImport "DataImport/DataImport.vcxproj")
a5_add_vcproj(FModSound "FModSound/FModSound.vcxproj")
a5_add_vcproj(FileIO "FileIO/FileIO.vcxproj")
a5_add_vcproj(FontGen "FontGen/FontGen.vcxproj")
a5_add_vcproj(Game "Game/Game.vcxproj")
a5_add_vcproj(GfxTest "GfxTest/GfxTest.vcxproj")
a5_add_vcproj(Image "Image/Image.vcxproj")
a5_add_vcproj(Input "Input/Input.vcxproj")
a5_add_vcproj(Main "Main/Main.vcxproj")
a5_add_vcproj(MapEdit "MapEdit/MapEdit.vcxproj")
a5_add_vcproj(MemoryMngr "MemoryMngr/MemoryMngr.vcxproj")
a5_add_vcproj(Misc "Misc/Misc.vcxproj")
a5_add_vcproj(PkgBuilder "PkgBuilder/PkgBuilder.vcxproj")
a5_add_vcproj(Script "Script/Script.vcxproj")
a5_add_vcproj(ShaderCompiler "ShaderCompiler/ShaderCompiler.vcxproj")
a5_add_vcproj(Sound "Sound/Sound.vcxproj")
a5_add_vcproj(TexConv "TexConv/TexConv.vcxproj")
a5_add_vcproj(libpng "libpng/libpng.vcxproj")
a5_add_vcproj(zlib "zlib/zlib.vcxproj")
a5_add_vcproj(LSConverter "LSConverter/LSConverter.vcxproj")
a5_add_vcproj(OpenDynamix "OpenDynamix/OpenDynamix.vcxproj")
a5_add_vcproj(TexMipStrip "TexMipStrip/TexMipStrip.vcxproj")
a5_add_vcproj(Scintilla "Scintilla/Scintilla.vcxproj")
a5_add_vcproj(MemoryMngrDll "MemoryMngrDll/MemoryMngrDll.vcxproj")
a5_add_vcproj(MiscDll "MiscDll/MiscDll.vcxproj")
a5_add_vcproj(ADOImport "ADOImport/ADOImport.vcxproj")
a5_add_vcproj(ADOFake "ADOFake/ADOFake.vcxproj")

# Extra project present in folder but not listed in A5.sln
a5_add_vcproj(AItest "AItest/AItest.vcxproj")
# AItest has only Debug and Release; map the additional solution configurations.
set_target_properties(AItest PROPERTIES
  MAP_IMPORTED_CONFIG_FASTDEBUG Debug
  MAP_IMPORTED_CONFIG_RELEASEDLL Release)

# Dependencies mirrored from A5.sln (GlobalSection(ProjectDependencies))
add_dependencies(DBFormat MemoryMngr ADOImport Misc FileIO)
add_dependencies(DataImport DBFormat Misc MemoryMngr FileIO)
add_dependencies(FModSound MemoryMngr FileIO Misc)
add_dependencies(FileIO MemoryMngr Misc)
add_dependencies(FontGen Image MemoryMngr libpng Misc FileIO zlib)
add_dependencies(Game Input Misc Script FModSound Main MemoryMngr ADOImport MiscDll FileIO DBFormat)
add_dependencies(GfxTest FileIO Input MemoryMngr Misc)
add_dependencies(Image zlib MemoryMngr)
add_dependencies(Input FileIO MemoryMngr MiscDll Misc)
add_dependencies(Main DBFormat FModSound Input MemoryMngr Misc Script ADOImport MiscDll FileIO)
add_dependencies(MapEdit Misc Script Scintilla Image OpenDynamix FModSound DBFormat FileIO MemoryMngr Input)
add_dependencies(MemoryMngr MemoryMngrDll)
add_dependencies(PkgBuilder Misc MemoryMngr FileIO)
add_dependencies(Script MiscDll MemoryMngr Input Misc FileIO)
add_dependencies(ShaderCompiler FileIO MemoryMngr Misc)
add_dependencies(Sound FileIO MemoryMngr FModSound)
add_dependencies(TexConv libpng MemoryMngr Misc zlib FileIO Image)
add_dependencies(libpng zlib MemoryMngr)
add_dependencies(zlib MemoryMngr)
add_dependencies(LSConverter FileIO MemoryMngr Misc libpng Image)
add_dependencies(OpenDynamix MemoryMngr)
add_dependencies(TexMipStrip FileIO MemoryMngr Misc Image)
add_dependencies(Scintilla MemoryMngr)
add_dependencies(MiscDll FileIO Misc MemoryMngr)
add_dependencies(ADOImport MemoryMngr FileIO Misc)
add_dependencies(ADOFake MemoryMngr Misc)
