# The name of the operating system for which CMake is to build
if (${CMAKE_SYSTEM_NAME} MATCHES "Windows")
    set(IS_WINDOWS TRUE)
elseif (${CMAKE_SYSTEM_NAME} MATCHES "Linux")
    set(IS_LINUX TRUE)
elseif (${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
    set(IS_MACOSX TRUE)
endif()

# compiler type
if (CMAKE_COMPILER_IS_GNUCXX)
    set(IS_GNU TRUE)
elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "AppleClang")
    set(IS_CLANG TRUE)
elseif(MSVC)
    set(IS_MSVC TRUE)
endif()

# Initialize a variable to accumulate an rpath.  The origin is the
# RUNTIME DESTINATION of the target.  If not absolute it's appended
# to CMAKE_INSTALL_PREFIX.
function(init_rpath rpathRef origin)
    if(NOT IS_ABSOLUTE ${origin})
        if(DEFINED INSTALL_DIR_SUFFIX)
            set(origin "${CMAKE_INSTALL_PREFIX}/${INSTALL_DIR_SUFFIX}/${origin}")
        else()
            set(origin "${CMAKE_INSTALL_PREFIX}/${origin}")
        endif()
    endif()
    # add_rpath uses REALPATH, so we must make sure we always
    # do so here too, to get the right relative path
    get_filename_component(origin "${origin}" REALPATH)
    set(${rpathRef} "${origin}" PARENT_SCOPE)
endfunction()

# Add a relative target path to the rpath.  If target is absolute compute
# and add a relative path from the origin to the target.
function(add_rpath rpathRef target)
    if(IS_ABSOLUTE "${target}")
        # init_rpath calls get_filename_component([...] REALPATH), which does
        # symlink resolution, so we must do the same, otherwise relative path
        # determination below will fail.
        get_filename_component(target "${target}" REALPATH)
        # Make target relative to $ORIGIN (which is the first element in
        # rpath when initialized with init_rpath()).
        list(GET ${rpathRef} 0 origin)
        file(RELATIVE_PATH
            target
            "${origin}"
            "${target}"
        )
        if("x${target}" STREQUAL "x")
            set(target ".")
        endif()
    endif()
    file(TO_CMAKE_PATH "${target}" target)
    set(new_rpath "${${rpathRef}}")
    list(APPEND new_rpath "$ORIGIN/${target}")
    set(${rpathRef} "${new_rpath}" PARENT_SCOPE)
endfunction()

function(install_rpath rpathRef NAME)
    # Get and remove the origin.
    list(GET ${rpathRef} 0 origin)
    set(rpath ${${rpathRef}})
    list(REMOVE_AT rpath 0)

    # Canonicalize and uniquify paths.
    set(final "")
    foreach(path ${rpath})
        # Replace $ORIGIN with @loader_path
        if(IS_MACOSX)
            if("${path}/" MATCHES "^[$]ORIGIN/")
                # Replace with origin path.
                string(REPLACE "$ORIGIN/" "@loader_path/" path "${path}/")
            endif()
        endif()

        # Strip trailing slashes.
        string(REGEX REPLACE "/+$" "" path "${path}")

        # Ignore paths we already have.
        if (NOT ";${final};" MATCHES ";${path};")
            list(APPEND final "${path}")
        endif()
    endforeach()

    set_target_properties(${NAME}
        PROPERTIES
            INSTALL_RPATH_USE_LINK_PATH TRUE
            INSTALL_RPATH "${final}"
    )
endfunction()