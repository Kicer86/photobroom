
vcpkg_from_github(
    OUT_SOURCE_PATH SOURCE_PATH
    REPO Kicer86/openlibrary
    REF 29201ad485964de3de5b1a331fe0b14f864bb092 #v2.4.0
    SHA512 2e6db36ed4c50234956675aa66538790dca44e44f58329f9d85f3f40d4f47b8ebb5b0be1f3654d20bf207c19a48d930a91a126576180e85ea612728623531032
)

vcpkg_configure_cmake(
    SOURCE_PATH ${SOURCE_PATH}
    PREFER_NINJA
    OPTIONS -DCMAKE_PREFIX_PATH=C:/Qt/6.0/msvc2019_64
)

vcpkg_install_cmake()
vcpkg_fixup_cmake_targets(CONFIG_PATH lib/cmake/OpenLibrary)
vcpkg_copy_pdbs()

# Clean
file(REMOVE_RECURSE ${CURRENT_PACKAGES_DIR}/debug/include ${CURRENT_PACKAGES_DIR}/debug/share)
file(TOUCH ${CURRENT_PACKAGES_DIR}/share/openlibrary/copyright)
