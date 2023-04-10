vcpkg_from_github(
    OUT_SOURCE_PATH SOURCE_PATH
    REPO copperspice/cs_libguarded
    REF libguarded-1.4.0
    SHA512 7514f9800f5d8c7419cfa4586c8131f842fe7981e033ab9883d92756cbf1444b54a2f47d749c33d07bd6a4f459ad4dbf88d3264deb9e50ca6ae098a39c910c84
    HEAD_REF master
)

vcpkg_cmake_configure(
    SOURCE_PATH "${SOURCE_PATH}"
)

vcpkg_cmake_install()

vcpkg_cmake_config_fixup(CONFIG_PATH cmake/CsLibGuarded)

file(REMOVE_RECURSE "${CURRENT_PACKAGES_DIR}/debug")
