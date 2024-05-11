
# CMake script preparing build environment for windows
# http://stackoverflow.com/questions/17446981/cmake-externalproject-add-and-findpackage


function(addDependenciesInstallStep)

    find_package(Qt6 REQUIRED COMPONENTS Core)
    get_property(qt_moc_path TARGET Qt6::moc PROPERTY LOCATION)
    get_filename_component(qt_bin_dir ${qt_moc_path} DIRECTORY)

    find_program(WINDEPLOY windeployqt
        HINTS ${qt_bin_dir}
        REQUIRED
    )

    get_filename_component(WINDEPLOY_DIR ${WINDEPLOY} DIRECTORY)

    configure_file(${CMAKE_CURRENT_SOURCE_DIR}/cmake/system/win32_deps.cmake.in win32_deps.cmake @ONLY)

    include(${CMAKE_CURRENT_BINARY_DIR}/win32_deps.cmake)

endfunction(addDependenciesInstallStep)


#install dependencies
addDependenciesInstallStep()

#uninstall previous version of photo broom
set(CPACK_NSIS_ENABLE_UNINSTALL_BEFORE_INSTALL ON)

#define installation locations on windows (remove any references to version - only project name)
set(CPACK_PACKAGE_INSTALL_DIRECTORY     ${PROJECT_NAME})
set(CPACK_PACKAGE_INSTALL_REGISTRY_KEY  ${PROJECT_NAME})

#define Start menu
set(CPACK_NSIS_EXECUTABLES_DIRECTORY ".")
set(CPACK_PACKAGE_EXECUTABLES
    photo_broom "Photo Broom"
)

if(PHOTO_BROOM_BUILD_ID)
    set(CPACK_PACKAGE_VERSION "${PROJECT_VERSION}-b${PHOTO_BROOM_BUILD_ID}")
endif()
