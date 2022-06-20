cmake_minimum_required(VERSION 3.13...3.21)

project(FbxViz)

include(cmake/utils.cmake)

#------------------------------------------------------------------------------
# options
#------------------------------------------------------------------------------
if( IS_MACOSX )
    option(BUILD_UB2 "Build Universal Binary 2 (UB2) Intel64+arm64" ON)
endif()

#------------------------------------------------------------------------------
# utilities and compiler configuration
#-----------------------------------------------s-------------------------------
if( IS_MACOSX )
    if (BUILD_UB2)
        set(CMAKE_OSX_ARCHITECTURES "x86_64;arm64")
    else()
        set(CMAKE_OSX_ARCHITECTURES "x86_64")
    endif()
endif()

#------------------------------------------------------------------------------
# includes
#------------------------------------------------------------------------------
include(cmake/compiler_config.cmake)
include(cmake/fmt.cmake)

#------------------------------------------------------------------------------
# global options
#------------------------------------------------------------------------------
if( IS_LINUX )
    # Use RUNPATH instead of RPATH for all shared libs and executables on Linux
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -Wl,--enable-new-dtags")
    set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -Wl,--enable-new-dtags")
endif()

set(CMAKE_MODULE_PATH
    ${CMAKE_MODULE_PATH}
    ${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules
)

# enable assert in RelwithDebInfo
string( REPLACE "/DNDEBUG" "" CMAKE_CXX_FLAGS_RELWITHDEBINFO "${CMAKE_CXX_FLAGS_RELWITHDEBINFO}")

#------------------------------------------------------------------------------
# modules and definitions
#------------------------------------------------------------------------------
find_package(FBX REQUIRED)

if( IS_LINUX )
    find_package(LibXml2 REQUIRED)
    if(NOT LibXml2_FOUND)
        message(STATUS "LibXml2 Library was not found!!!")
    endif()
endif()

if( NOT FBX_FOUND )
    message(STATUS "FBX Library was not found!!!")
endif()

#------------------------------------------------------------------------------
# subdirectories
#------------------------------------------------------------------------------
add_subdirectory(src)
