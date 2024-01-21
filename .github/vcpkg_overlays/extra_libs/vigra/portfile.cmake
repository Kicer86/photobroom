
vcpkg_from_github(
    OUT_SOURCE_PATH SOURCE_PATH
    REPO ukoethe/vigra
    REF "refs/tags/Version-1-11-2"
    SHA512 beeea9f503447218d34c0f901ffee7735589c34e9e36dc8cd440bca554b7ee8841885e5a146e97bb0065cbc66672ede8510f37667944c127818a18c7258581e5
    HEAD_REF master
)

vcpkg_cmake_configure(
    SOURCE_PATH "${SOURCE_PATH}"
)

vcpkg_cmake_install()
vcpkg_install_copyright(FILE_LIST "${SOURCE_PATH}/LICENSE.txt")
vcpkg_copy_pdbs()

file(REMOVE_RECURSE "${CURRENT_PACKAGES_DIR}/debug/include")
