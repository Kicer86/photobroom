
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

    if(NOT EXISTS ${CMAKE_CURRENT_BINARY_DIR}/tools/ffmpeg-release-essentials.7z)
        message("Downloading FFMpeg")
        file(DOWNLOAD
            https://www.gyan.dev/ffmpeg/builds/ffmpeg-release-essentials.7z ${CMAKE_CURRENT_BINARY_DIR}/tools/ffmpeg-release-essentials.7z
            SHOW_PROGRESS
        )
        file(ARCHIVE_EXTRACT
            INPUT ${CMAKE_CURRENT_BINARY_DIR}/tools/ffmpeg-release-essentials.7z
            DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/tools
        )
    endif()

    install(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/tools/ffmpeg-5.1-essentials_build/   # version :/ not nice to have it here
        DESTINATION tools/FFMpeg
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

    if(NOT EXISTS ${CMAKE_CURRENT_BINARY_DIR}/tools/Hugin-2020.0.0-win64.msi)
        message("Downloading Hugin")
        file(DOWNLOAD
            https://netcologne.dl.sourceforge.net/project/hugin/hugin/hugin-2020.0/Hugin-2020.0.0-win64.msi ${CMAKE_CURRENT_BINARY_DIR}/tools/Hugin-2020.0.0-win64.msi
            SHOW_PROGRESS
        )

        file(TO_NATIVE_PATH "${CMAKE_BINARY_DIR}/tools/Hugin-2020.0.0-win64.msi" hugin_source)
        file(TO_NATIVE_PATH "${CMAKE_BINARY_DIR}/tools/Hugin" hugin_destination)
        execute_process(
            COMMAND msiexec /a ${hugin_source} /quiet TARGETDIR=${hugin_destination}
        )
    endif()

    install(FILES ${CMAKE_CURRENT_BINARY_DIR}/tools/Hugin/Hugin/bin/align_image_stack.exe
        DESTINATION tools/Hugin
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
