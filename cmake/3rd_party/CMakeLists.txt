
project(third_party_libraries)

cmake_minimum_required(VERSION 3.0)

include(ExternalProject)

set(INSTALL_LOCATION ${PROJECT_BINARY_DIR}/install)

# KDE5
ExternalProject_Add(extra-cmake-modules
    GIT_REPOSITORY git://anongit.kde.org/extra-cmake-modules.git
    PREFIX ${PROJECT_BINARY_DIR}
    CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${INSTALL_LOCATION}
    )

ExternalProject_Add(kcoreaddons
    DEPENDS extra-cmake-modules
    GIT_REPOSITORY git://anongit.kde.org/kcoreaddons.git
    PREFIX ${PROJECT_BINARY_DIR}
    CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${INSTALL_LOCATION}
               -DECM_DIR=${INSTALL_LOCATION}/share/ECM/cmake
    )

ExternalProject_Add(kwindowsystem
    DEPENDS extra-cmake-modules
    GIT_REPOSITORY git://anongit.kde.org/kwindowsystem.git
    PREFIX ${PROJECT_BINARY_DIR}
    CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${INSTALL_LOCATION}
               -DECM_DIR=${INSTALL_LOCATION}/share/ECM/cmake
    )
    
ExternalProject_Add(kcrash
    DEPENDS extra-cmake-modules
            kcoreaddons
            kwindowsystem
    GIT_REPOSITORY git://anongit.kde.org/kcrash.git
    PREFIX ${PROJECT_BINARY_DIR}
    CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${INSTALL_LOCATION}
               -DECM_DIR=${INSTALL_LOCATION}/share/ECM/cmake
               -DKF5CoreAddons_DIR=${INSTALL_LOCATION}/lib/cmake/KF5CoreAddons
               -DKF5WindowSystem_DIR=${INSTALL_LOCATION}/lib/cmake/KF5WindowSystem
    )

# EASYEXIF
ExternalProject_Add(easyexif
    GIT_REPOSITORY https://github.com/mayanklahiri/easyexif.git
    PATCH_COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_SOURCE_DIR}/easyexif/CMakeLists.txt ${PROJECT_BINARY_DIR}/src/easyexif
    PREFIX ${PROJECT_BINARY_DIR} 
    CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${INSTALL_LOCATION}
    )
    
# HASH-LIBRARY
ExternalProject_Add(hash-library
    GIT_REPOSITORY http://create.stephan-brumme.com/hash-library/.git
    PREFIX ${PROJECT_BINARY_DIR}
    CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${INSTALL_LOCATION}
    )
    
# JSONCPP
ExternalProject_Add(jsoncpp
    GIT_REPOSITORY https://github.com/open-source-parsers/jsoncpp.git
    PREFIX ${PROJECT_BINARY_DIR}
    CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${INSTALL_LOCATION}
    )