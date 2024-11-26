#=========================================================================================
#  __   __  ___        ___  ______    _____   __   __   ________  ___      ___  _______
# |  | |  | \  \  /\  /  / |   _  \  |   __| |  | |  | |__    __| \  \    /  / /   _   \
# |  |_|  |  \  \/  \/  /  |  | |  | |  |    |  |_|  |    |  |     \  \  /  /  |  | |  |
# |   _   |   \        /   |  | |  | |  |    |   _   |    |  |      \  \/  /   |  |_|  |
# |  | |  |    \  /\  /    |  |_|  | |  |__  |  | |  |  __|  |__     \    /    |   _   |
# |__| |__|     \/  \/     |______/  |_____| |__| |__| |________|     \__/     |__| |__|
#
#=========================================================================================

cmake_minimum_required(VERSION 3.15.0)
set(CMAKE_CONFIGURATION_TYPES Debug Release CACHE STRING INTERNAL FORCE)

project(rest_control)

if ( MSVC_VERSION LESS 1920 )
    message(FATAL_ERROR "MSVS Version not supported: ${MSVC_VERSION}. Supported versions are 1920 (MSVS 2019) or greater")
endif()

# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_COLOR_MAKEFILE TRUE)
set(CMAKE_VERBOSE_MAKEFILE OFF)

SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR})
SET(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR})

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake/")
include(ccmake)

# From ccmake
setPlatformInfo()
addQT5()

# Enable searching for libraries in lib64 directories during 64-bit builds.
set_property(GLOBAL PROPERTY FIND_LIBRARY_USE_LIB64_PATHS TRUE)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Add the following compiling options to all projects:
if(MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /bigobj /MP /openmp-")
    set(CMAKE_EXE_LINKER_FLAGS_RELEASE "${CMAKE_EXE_LINKER_FLAGS_RELEASE} /INCREMENTAL:NO /ENTRY:\"mainCRTStartup\"")
    set(CMAKE_EXE_LINKER_FLAGS_DEBUG "${CMAKE_EXE_LINKER_FLAGS_DEBUG} /INCREMENTAL:NO" )
endif()

cmake_policy(SET CMP0020 NEW)

EXECUTE_PROCESS( COMMAND "cmake" "-E" "make_directory" "${EXECUTABLE_OUTPUT_PATH}/Release")
EXECUTE_PROCESS( COMMAND "cmake" "-E" "make_directory" "${EXECUTABLE_OUTPUT_PATH}/Debug")

# Include directory
include_directories(${PROJECT_BINARY_DIR})

# Include SDL dll and library
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/libs/SDL2-2.0.8/include)
set(SDL_LIB optimized ${CMAKE_CURRENT_SOURCE_DIR}/libs/SDL2-2.0.8/lib/x64/SDL2.lib)
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/libs/SDL2-2.0.8/lib/x64/SDL2.dll DESTINATION "${EXECUTABLE_OUTPUT_PATH}/Release")

# Include QT5 dlls 
foreach(dll ${QT5_REQUIRED_LIBS})
    file(COPY "${CMAKE_PREFIX_PATH}/bin/${dll}.dll" DESTINATION "${EXECUTABLE_OUTPUT_PATH}/Release")
    file(COPY "${CMAKE_PREFIX_PATH}/bin/${dll}d.dll" DESTINATION "${EXECUTABLE_OUTPUT_PATH}/Debug")
endforeach()

#Copy required plugins
file(COPY "${CMAKE_PREFIX_PATH}/plugins/platforms/qwindows.dll" DESTINATION "${EXECUTABLE_OUTPUT_PATH}/Release/plugins/platforms")
file(COPY "${CMAKE_PREFIX_PATH}/plugins/platforms/qwindowsd.dll" DESTINATION "${EXECUTABLE_OUTPUT_PATH}/Debug/plugins/platforms")
file(COPY "${CMAKE_PREFIX_PATH}/plugins/platforminputcontexts/qtvirtualkeyboardplugin.dll" DESTINATION "${EXECUTABLE_OUTPUT_PATH}/Release/plugins/platforminputcontexts")
file(COPY "${CMAKE_PREFIX_PATH}/plugins/platforminputcontexts/qtvirtualkeyboardplugind.dll" DESTINATION "${EXECUTABLE_OUTPUT_PATH}/Debug/plugins/platforminputcontexts")

# Windows specific defs
add_definitions(-D${MSVC_CXX_ARCHITECTURE_ID})
add_definitions(-DWIN32 -DWINVER=0x0A00 -D_WIN32_WINNT=0x0A00 -DWIN32_LEAN_AND_MEAN -DVC_EXTRALEAN)
# Thrid party specific defs
add_definitions(-D_NEWBOOST_ -DNOMINMAX -D_USE_FFTW_LIB=0 -DUSE_IMPORT_EXPORT)
# Compiler warnings def
add_definitions(-D_SCL_SECURE_NO_WARNINGS -D_CRT_SECURE_NO_DEPRECATE -D_SILENCE_STDEXT_HASH_DEPRECATION_WARNINGS)
#add_definitions(-D__STDC_LIMIT_MACROS)

# 4244: '=' : conversion from 'double' to 'int', possible loss of data
# 4274: #ident ignored; see documentation for #pragma comment(exestr, 'string')
# 4290: C++ exception specification ignored except to indicate a function is not __declspec(nothrow)
# 4996: By default, this function always fails with operation_not_supported when used on Windows XP, Windows Server 2003, or earlier.
set(MSWARNINGS 4244 4274 4290 4996)
foreach(wwarning ${MSWARNINGS})
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /wd${wwarning} ")
endforeach()

set(rc_version_info "${CMAKE_CURRENT_SOURCE_DIR}/installer/version_info.ps1")
set (rc_version_cmd powershell.exe ARGS -ExecutionPolicy RemoteSigned -File \"${rc_version_info}\"
  gitversion \"${PROJECT_BINARY_DIR}/\" \"${CMAKE_CURRENT_SOURCE_DIR}/\"
  COMMENT "Generating version information"
  )

set (rc_windows10_manifest_cmd mt.exe -manifest \"${CMAKE_CURRENT_SOURCE_DIR}/src/rc/Windows10.manifest\"
    -inputresource:"$(TargetDir)$(TargetFileName)"\\\;\#1
    -outputresource:"$(TargetDir)$(TargetFileName)"\\\;\#1
    COMMENT "Adding Windows 10 application manifest"
    )

set (WIN32_LIBS wsock32 ws2_32 wldap32)

add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/src")

