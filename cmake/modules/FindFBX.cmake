# Copyright (C) 2022 Hamed Sabri
#
# All rights reserved.
#
# Redistribution and use in source and binary forms, with or without modification,
# are permitted provided that the following conditions are met:
#
# 1. Redistributions of source code must retain the above copyright notice, this
# list of conditions and the following disclaimer.
# 2. Redistributions in binary form must reproduce the above copyright notice,
# this list of conditions and the following disclaimer in the documentation
# and/or other materials provided with the distribution.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
# ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
# WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
# DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
# ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
# (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
# LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
# ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
# (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
# SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

# Basic module to find FBXSDK.
# This module searches for a valid FBXSDK installation.
#
# Accepted variable as input:
#
#    FBX_LOCATION - (as a CMake or environment variable)
#               The root directory of the FBX SDK install.
#
# Variables that will be defined:
#
#    FBX_FOUND           Defined if a FBX installation has been detected
#    FBX_INCLUDE_DIR     Path to the FBX include directory
#    FBX_LIBRARIES       Path to FBX library(s)
#    FBX_VERSION         FBX version (major.minor.revision) from fbxsdk.h
#

# ========================================
# FBX_INCLUDE_DIR
# ========================================

find_path(FBX_INCLUDE_DIR
        fbxsdk.h
    HINTS
        $ENV{FBX_LOCATION}
        ${FBX_LOCATION}
    PATH_SUFFIXES
        include
    DOC
        "FBX header path"
)
mark_as_advanced(FBX_INCLUDE_DIR)

# ========================================
# FBX_VERSION
# ========================================
if(FBX_INCLUDE_DIR AND EXISTS "${FBX_INCLUDE_DIR}/fbxsdk/fbxsdk_version.h")

    file(READ "${FBX_INCLUDE_DIR}/fbxsdk/fbxsdk_version.h" ver)

    string(REGEX MATCH "#define[ ]+FBXSDK_VERSION_MAJOR+    ([0-9]*)" _ ${ver})
    set(ver_major ${CMAKE_MATCH_1})

    string(REGEX MATCH "#define[ ]+FBXSDK_VERSION_MINOR+    ([0-9]*)" _ ${ver})
    set(ver_minor ${CMAKE_MATCH_1})

    string(REGEX MATCH "#define[ ]+FBXSDK_VERSION_POINT+    ([0-9]*)" _ ${ver})
    set(ver_point ${CMAKE_MATCH_1})

    set(FBX_VERSION ${ver_major}.${ver_minor}.${ver_point})
endif()

# ========================================
# FBX_LIBRARIES
# ========================================
function(find_fbx_library name lib suffix)
    if(IS_WINDOWS)
        set(VS_PREFIX vs2017)
    endif()
    find_library(${name}
        NAMES ${lib}
        HINTS 
            $ENV{FBX_LOCATION}
            ${FBX_LOCATION}
        PATH_SUFFIXES
            lib
            lib/${VS_PREFIX}/x64/${suffix}
            lib/clang/${suffix}
            lib/gcc/x64/${suffix}
    )
    mark_as_advanced(${name})
endfunction()

function(find_fbx_file name fileName suffix)
    if(IS_WINDOWS)
        set(VS_PREFIX vs2017)
    endif()
    find_file(${name}
        NAMES ${fileName}
        HINTS 
            $ENV{FBX_LOCATION}
            ${FBX_LOCATION}
        PATH_SUFFIXES
            lib
            lib/${VS_PREFIX}/x64/${suffix}
            lib/clang/${suffix}
            lib/gcc/x64/${suffix}
    )
    mark_as_advanced(${name})
endfunction()

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    if ( IS_LINUX )
        find_fbx_library(FBX_LIBRARY_DEBUG  libfbxsdk.so debug)
    elseif( IS_MACOSX )
        find_fbx_library(FBX_LIBRARY_DEBUG  libfbxsdk.dylib debug)
    elseif( IS_WINDOWS )
        find_fbx_library(FBX_LIBRARY_DEBUG  libfbxsdk debug)
        find_fbx_file(FBX_LIBRARY_DLL libfbxsdk.dll debug)
    endif()

    set( FBX_LIBRARIES ${FBX_LIBRARY_DEBUG} )

else()
    if ( IS_LINUX )
        find_fbx_library(FBX_LIBRARIES  libfbxsdk.so release)
    elseif( IS_MACOSX )
        find_fbx_library(FBX_LIBRARIES  libfbxsdk.dylib release)
    elseif( IS_WINDOWS )
        find_fbx_library(FBX_LIBRARIES  libfbxsdk release)
        find_fbx_file(FBX_LIBRARY_DLL libfbxsdk.dll release)
    endif()
    
    set( FBX_LIBRARIES ${FBX_LIBRARY} )

endif()

include(FindPackageHandleStandardArgs)

if ( IS_WINDOWS )
    find_package_handle_standard_args(FBX
        REQUIRED_VARS
            FBX_INCLUDE_DIR
            FBX_LIBRARIES
            FBX_LIBRARY_DLL
        VERSION_VAR
            FBX_VERSION
    )
else()
    find_package_handle_standard_args(FBX
        REQUIRED_VARS
            FBX_INCLUDE_DIR
            FBX_LIBRARIES
        VERSION_VAR
            FBX_VERSION
    )
endif()

if(FBX_FOUND)
    message(STATUS "FBX include dir: ${FBX_INCLUDE_DIR}")
    message(STATUS "FBX library: ${FBX_LIBRARIES}")
    message(STATUS "FBX version: ${FBX_VERSION}")
endif()
