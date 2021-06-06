
vcpkg_from_github(
    OUT_SOURCE_PATH SOURCE_PATH
    REPO Kicer86/openlibrary
    REF 94010cfe291b9dc283a94b440b032b478dfb1a9e #v2.4.0
    SHA512 cf583f4e034dd3a5ecf4a4fbd6cf7f54154089dfac51ee2688eac89354dbe73bf8d54cf1f022263acef65826382aaf586c9e4be280346112ef50bf6a4356715d
)

vcpkg_configure_cmake(
    SOURCE_PATH ${SOURCE_PATH}
    PREFER_NINJA
)

vcpkg_install_cmake()
vcpkg_fixup_cmake_targets(CONFIG_PATH lib/cmake/OpenLibrary)
vcpkg_copy_pdbs()

# Clean
file(REMOVE_RECURSE ${CURRENT_PACKAGES_DIR}/debug/include ${CURRENT_PACKAGES_DIR}/debug/share)
file(TOUCH ${CURRENT_PACKAGES_DIR}/share/openlibrary/copyright)
