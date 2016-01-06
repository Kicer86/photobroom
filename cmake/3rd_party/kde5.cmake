
include(ExternalProject)

set(INSTALL_LOCATION ${PROJECT_BINARY_DIR}/3rd_party/external/install)

ExternalProject_Add(extra-cmake-modules
    GIT_REPOSITORY git://anongit.kde.org/extra-cmake-modules.git
    PREFIX  ${PROJECT_BINARY_DIR}/3rd_party/external
    CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${INSTALL_LOCATION}
    )

ExternalProject_Add(kcoreaddons
    DEPENDS extra-cmake-modules
    GIT_REPOSITORY git://anongit.kde.org/kcoreaddons.git
    PREFIX  ${PROJECT_BINARY_DIR}/3rd_party/external
    CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${INSTALL_LOCATION}
               -DECM_DIR=${INSTALL_LOCATION}/share/ECM/cmake
    )

ExternalProject_Add(kwindowsystem
    DEPENDS extra-cmake-modules
    GIT_REPOSITORY git://anongit.kde.org/kwindowsystem.git
    PREFIX  ${PROJECT_BINARY_DIR}/3rd_party/external
    CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${INSTALL_LOCATION}
               -DECM_DIR=${INSTALL_LOCATION}/share/ECM/cmake
    )
    
ExternalProject_Add(kcrash
    DEPENDS extra-cmake-modules
            kcoreaddons
            kwindowsystem
    GIT_REPOSITORY git://anongit.kde.org/kcrash.git
    PREFIX  ${PROJECT_BINARY_DIR}/3rd_party/external
    CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${INSTALL_LOCATION}
               -DECM_DIR=${INSTALL_LOCATION}/share/ECM/cmake
               -DKF5CoreAddons_DIR=${INSTALL_LOCATION}/lib/cmake/KF5CoreAddons
               -DKF5WindowSystem_DIR=${INSTALL_LOCATION}/lib/cmake/KF5WindowSystem
    )
