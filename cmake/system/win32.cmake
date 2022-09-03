
# CMake script preparing build environment for windows
# http://stackoverflow.com/questions/17446981/cmake-externalproject-add-and-findpackage


function(download_tools)
    if(NOT EXISTS ${CMAKE_CURRENT_BINARY_DIR}/tools/ImageMagick-7.1.0-portable-Q16-x64.zip)
        message("Downloading ImageMagick")
        file(DOWNLOAD
            https://imagemagick.org/archive/binaries/ImageMagick-7.1.0-portable-Q16-x64.zip ${CMAKE_CURRENT_BINARY_DIR}/tools/ImageMagick-7.1.0-portable-Q16-x64.zip
            SHOW_PROGRESS
        )
        file(ARCHIVE_EXTRACT
            INPUT ${CMAKE_CURRENT_BINARY_DIR}/tools/ImageMagick-7.1.0-portable-Q16-x64.zip
            DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/tools/ImageMagick
        )
    endif()

    install(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/tools/ImageMagick/
        DESTINATION tools/ImageMagick
    )

    if(NOT EXISTS ${CMAKE_CURRENT_BINARY_DIR}/tools/exiftool-12.35.zip)
        message("Downloading ExifTool")
        file(DOWNLOAD
            https://exiftool.org/exiftool-12.35.zip ${CMAKE_CURRENT_BINARY_DIR}/tools/exiftool-12.35.zip
            SHOW_PROGRESS
        )
        file(ARCHIVE_EXTRACT
            INPUT ${CMAKE_CURRENT_BINARY_DIR}/tools/exiftool-12.35.zip
            DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/tools
        )
    endif()

    install(FILES ${CMAKE_CURRENT_BINARY_DIR}/tools/exiftool\(-k\).exe
        DESTINATION tools/ExifTool/
        RENAME exiftool.exe
    )

    if(NOT EXISTS ${CMAKE_CURRENT_BINARY_DIR}/tools/Hugin-2021.0.0-win64.msi)
        message("Downloading Hugin")
        file(DOWNLOAD
            https://netix.dl.sourceforge.net/project/hugin/hugin/hugin-2021.0/Hugin-2021.0.0-win64.msi ${CMAKE_CURRENT_BINARY_DIR}/tools/Hugin-2021.0.0-win64.msi
            SHOW_PROGRESS
        )

        find_program(APP_7Z 7z REQUIRED)
        execute_process(
            COMMAND
                ${APP_7Z} e
                -o${CMAKE_CURRENT_BINARY_DIR}/tools/Hugin
                ${CMAKE_CURRENT_BINARY_DIR}/tools/Hugin-2021.0.0-win64.msi
        )
    endif()

    install(FILES ${CMAKE_CURRENT_BINARY_DIR}/tools/Hugin/CM_FP_bin.align_image_stack.exe
        DESTINATION tools/Hugin
        RENAME align_image_stack.exe
    )

endfunction(download_tools)


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

#download tools
download_tools()

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
