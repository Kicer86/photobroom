
find_package(Qt5Core REQUIRED)

add_executable(deployer deployer.cpp)

target_link_libraries(deployer PRIVATE Qt5::Core)
                                    