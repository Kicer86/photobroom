
vcpkg_from_github(
    OUT_SOURCE_PATH SOURCE_PATH
    REPO Kicer86/openlibrary
    REF 7d59e0c5520a68af239cc76d8029206f8d5644b5 #v2.4.0
    SHA512 0c2ec7f677fca52798ec0a355a7a83dee6d7631e72c6985622bebdf5367fba0e0546388aada346b6dd98b1cb7ec24776010df9a1f0d33778cb4c64f88ff4395c
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
