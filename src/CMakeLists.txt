add_executable(${PROJECT_NAME})

# -----------------------------------------------------------------------------
# sources
# -----------------------------------------------------------------------------
target_sources(${PROJECT_NAME}
    PRIVATE
        main.cpp
        common.cpp
        curveInfo.cpp
)

# -----------------------------------------------------------------------------
# compiler configuration
# -----------------------------------------------------------------------------
compile_config(${PROJECT_NAME})

# -----------------------------------------------------------------------------
# properties
# -----------------------------------------------------------------------------

# -----------------------------------------------------------------------------
# include directories
# -----------------------------------------------------------------------------
target_include_directories(${PROJECT_NAME} 
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${FBX_INCLUDE_DIR}
)

# -----------------------------------------------------------------------------
# link libraries
# -----------------------------------------------------------------------------
target_link_libraries(${PROJECT_NAME}
    PRIVATE
        ${FBX_LIBRARIES}
        fmt
        $<$<BOOL:${IS_LINUX}>:LibXml2::LibXml2>
)

# -----------------------------------------------------------------------------
# compiler configuration
# -----------------------------------------------------------------------------
target_compile_definitions(${PROJECT_NAME}
    PRIVATE
        FBXSDK_NEW_API
        FBXSDK_SHARED
)

# -----------------------------------------------------------------------------
# run-time search paths
# -----------------------------------------------------------------------------
if( IS_LINUX )
    init_rpath(rpath ${PROJECT_NAME})
    add_rpath(rpath "${FBX_LOCATION}/lib")
    install_rpath(rpath ${PROJECT_NAME})
endif()

# -----------------------------------------------------------------------------
# subdirectories
# -----------------------------------------------------------------------------
add_subdirectory(graph)

# -----------------------------------------------------------------------------
# install
# -----------------------------------------------------------------------------
install(TARGETS ${PROJECT_NAME}
    LIBRARY
    DESTINATION ${CMAKE_INSTALL_PREFIX}
    RUNTIME
    DESTINATION ${CMAKE_INSTALL_PREFIX}
)

if( IS_WINDOWS )
    install(FILES $<TARGET_PDB_FILE:${PROJECT_NAME}> 
            DESTINATION ${CMAKE_INSTALL_PREFIX} OPTIONAL
    )

    install(FILES ${FBX_LIBRARY_DLL}
        DESTINATION ${CMAKE_INSTALL_PREFIX}
    )

else( IS_MACOSX )
    install(FILES ${FBX_LIBRARIES}
        DESTINATION ${CMAKE_INSTALL_PREFIX}
    )
endif()