
# openSSL for Windows: http://slproweb.com/products/Win32OpenSSL.html

find_package(OpenLibrary 2.0 REQUIRED putils)
find_package(Qt5Core REQUIRED)
find_package(Qt5Gui REQUIRED)
find_package(OpenMP)
find_exif_package()
find_cryptographic_package()       # OpenSSL or Nettle

include(${OPENLIBRARY_USE_FILE})
include(GenerateExportHeader)

include_directories(${CMAKE_BINARY_DIR}/exports ${CMAKE_SOURCE_DIR}/src)
include_directories(SYSTEM ${Qt5Core_INCLUDE_DIRS}
                           ${Qt5Gui_INCLUDE_DIRS}
                           ${OPENLIBRARY_INCLUDE_DIRS}
                           ${CRYPTOGRAPHIC_INCLUDE_DIR}
                           ${EXIF_INCLUDE_DIR})

# some classes in core are using interface types of database. Is it ok? - to consider
include_directories($<TARGET_PROPERTY:database,INTERFACE_INCLUDE_DIRECTORIES>)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CORE_SOURCES
    implementation/atagfeeder.cpp
    implementation/base_tags.cpp
    implementation/disk_observer.cpp
    implementation/hash_functions.cpp
    implementation/logger.cpp
    implementation/plugin_loader.cpp
    implementation/photos_manager.cpp
    implementation/tag.cpp
    implementation/tag_feeder_factory.cpp
    implementation/tag_updater.cpp
    implementation/task_executor.cpp
    implementation/time_guardian.cpp
)

set(CORE_HEADERS
    base_tags.hpp
    callback_ptr.hpp
    disk_observer.hpp
    hash_functions.hpp
    ilogger.hpp
    itagfeeder.hpp
    itask_executor.hpp
    iplugin_loader.hpp
    logger.hpp
    plugin_loader.hpp
    photos_manager.hpp
    tag.hpp
    tag_feeder_factory.hpp
    tag_updater.hpp
    task_executor.hpp
    time_guardian.hpp
    implementation/atagfeeder.hpp
    implementation/tree_iterator_base.hpp
    implementation/tree_flat_iterator.hpp
    implementation/tree_private.hpp
    implementation/tree_recursive_iterator.hpp
)

set(CORE_RESOURCE_FILES images/images.qrc)
set(CORE_MOC_FILES disk_observer.hpp)

qt5_add_resources(CORE_RESOURCES ${CORE_RESOURCE_FILES})
qt5_wrap_cpp(CORE_WRAPPED ${CORE_MOC_FILES})

if(EXIV2_FOUND)

    add_definitions(-DUSE_EXIV2)
    set(CORE_SOURCES ${CORE_SOURCES} implementation/exiv2_tag_feeder.cpp)
    set(CORE_HEADERS ${CORE_HEADERS} implementation/exiv2_tag_feeder.hpp)

elseif(EASYEXIF_FOUND)

    add_definitions(-DUSE_EASY_EXIF)
    set(CORE_SOURCES ${CORE_SOURCES} implementation/easy_exif_tag_feeder.cpp)
    set(CORE_HEADERS ${CORE_HEADERS} implementation/easy_exif_tag_feeder.hpp)

endif(EXIV2_FOUND)


if(OPENSSL_FOUND)
    add_definitions(-DUSE_OPENSSL)
elseif(NETTLE_FOUND)
    add_definitions(-DUSE_NETTLE)
endif(OPENSSL_FOUND)

#task executor uses openmp
if(OPENMP_FOUND)
    addSourceFlags(implementation/task_executor.cpp COMPILE_FLAGS ${OpenMP_CXX_FLAGS})
    add_definitions(-DUSE_OPENMP)
endif()

#addSourceFlags(implementation/photo_info.cpp COMPILE_FLAGS -fgnu-tm)

source_group(files REGULAR_EXPRESSION .*core.* )

add_library(core SHARED ${CORE_SOURCES} ${CORE_RESOURCES} ${CORE_HEADERS} ${CORE_WRAPPED})
target_link_libraries(core PRIVATE system ${OPENLIBRARY_LIBRARIES} Qt5::Core Qt5::Gui ${CRYPTOGRAPHIC_LIBRARIES} ${EXIF_LIBRARIES})
target_include_directories(core PUBLIC ${CMAKE_CURRENT_BINARY_DIR})

if(OPENMP_FOUND)
    if(MSVC)

    else()
        target_link_libraries(core PRIVATE ${OpenMP_CXX_FLAGS})
    endif(MSVC)
endif(OPENMP_FOUND)


generate_export_header(core)
hideSymbols(core)

install(TARGETS core RUNTIME DESTINATION ${PATH_LIBS}
                     LIBRARY DESTINATION ${PATH_LIBS})
