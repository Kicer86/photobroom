
vcpkg_from_github(
    OUT_SOURCE_PATH SOURCE_PATH
    REPO Kicer86/openlibrary
    REF 0f04f5bb43d007b21abbca035015c41f4d678be9 #v2.3.0
    SHA512 4d9561820039cae495db06c3b8d6f6382ef0309732b1655b31b6da3582201ed42effb1dedab922d63941c8f0e7da29c7d9777a89f963e2c559aff1b9b8122fda
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
