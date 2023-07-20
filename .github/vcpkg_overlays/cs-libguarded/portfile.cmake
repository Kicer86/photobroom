vcpkg_from_github(
    OUT_SOURCE_PATH SOURCE_PATH
    REPO copperspice/cs_libguarded
    REF libguarded-1.4.1
    SHA512 fb54cce1be0ecbd363cdf7a8bdfcddd540737419aea702a29e5ee37143434f7431de1edb085bdfc2dc1eded9749dab40afd634582479118d2202c8d46b196c7d
    HEAD_REF master
)

vcpkg_cmake_configure(
    SOURCE_PATH "${SOURCE_PATH}"
)

vcpkg_cmake_install()

vcpkg_cmake_config_fixup(CONFIG_PATH lib/cmake/CsLibGuarded/)

file(REMOVE_RECURSE "${CURRENT_PACKAGES_DIR}/debug")
