#------------------------------------------------------------------------------
# compiler flags/definitions
#------------------------------------------------------------------------------
set(MSVC_FLAGS
    # we want to be as strict as possible
    /W3
    /WX
    # enable pdb generation.
    /Zi
    # standards compliant.
    /Zc:rvalueCast
    # The /Zc:inline option strips out the "arch_ctor_<name>" symbols used for
    # library initialization by ARCH_CONSTRUCTOR starting in Visual Studio 2019,
    # causing release builds to fail. Disable the option for this and later
    # versions.
    #
    # For more details, see:
    # https://developercommunity.visualstudio.com/content/problem/914943/zcinline-removes-extern-symbols-inside-anonymous-n.html
    $<IF:$<VERSION_GREATER_EQUAL:${MSVC_VERSION},1920>,/Zc:inline-,/Zc:inline>
    # enable multiprocessor builds.
    /MP
    # enable exception handling.
    /EHsc
    # enable initialization order as a level 3 warning
    /w35038
    # disable warnings
    /wd4244
    /wd4267
    /wd4273
    /wd4305
    /wd4506
    /wd4996
    /wd4180
    # exporting STL classes
    /wd4251
)

set(MSVC_DEFINITIONS
    # Make sure WinDef.h does not define min and max macros which
    # will conflict with std::min() and std::max().
    NOMINMAX

    _CRT_SECURE_NO_WARNINGS
    _SCL_SECURE_NO_WARNINGS
)

#------------------------------------------------------------------------------
# compiler configuration
#------------------------------------------------------------------------------
# Do not use GNU extension 
# Use -std=c++11 instead of -std=gnu++11
set(CMAKE_CXX_EXTENSIONS OFF)

function(compile_config TARGET)
    target_compile_features(${TARGET} 
        PRIVATE
            cxx_std_17
    )
endfunction()
