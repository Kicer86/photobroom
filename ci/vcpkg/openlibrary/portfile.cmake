
vcpkg_from_github(
    OUT_SOURCE_PATH SOURCE_PATH
    REPO Kicer86/openlibrary
    REF 02c321d26f4113c173bdc82622f89cf8a228c84a #v2.4.0
    SHA512 59c9ca5ba8f944c843bf31704f62cf2f44a686cff627ca95adbd1bb4a53e14a97de040ac5e7e858742a33072578f3163ba5a3c2caa39c9dbfbe5c15e5387b7e9
)

vcpkg_configure_cmake(
    SOURCE_PATH ${SOURCE_PATH}
    PREFER_NINJA
    OPTIONS -DQt5Widgets_DIR="$ENV{Qt5Widgets_DIR}"
)

vcpkg_install_cmake()
vcpkg_fixup_cmake_targets(CONFIG_PATH lib/cmake/OpenLibrary)
vcpkg_copy_pdbs()

# Clean
file(REMOVE_RECURSE ${CURRENT_PACKAGES_DIR}/debug/include ${CURRENT_PACKAGES_DIR}/debug/share)
file(TOUCH ${CURRENT_PACKAGES_DIR}/share/openlibrary/copyright)

